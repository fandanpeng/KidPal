#!/usr/bin/env node
import {
  existsSync,
  lstatSync,
  mkdirSync,
  mkdtempSync,
  readFileSync,
  readdirSync,
  realpathSync,
  rmSync,
  statSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, isAbsolute, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

export const LIMITS = Object.freeze({
  futureSkewMs: 300_000,
  maxFacts: 128,
  maxAggregateBytes: 8_388_608,
  maxFactBytes: 262_144,
  tempCleanupMs: 86_400_000,
  maxResults: 64,
  perResultStdoutBytes: 16_384,
  perResultStderrBytes: 16_384,
  perResultSummaryBytes: 4_096,
  aggregateStdoutBytes: 131_072,
  aggregateStderrBytes: 131_072,
  aggregateSummaryBytes: 32_768,
  errorNameBytes: 128,
  errorMessageBytes: 8_192,
  errorFieldBytes: 4_096,
  errorEnvelopeBytes: 16_384,
  checkerStdoutBytes: 2_048,
  checkerStderrBytes: 8_192,
  parentStdoutBytes: 4_096,
  parentStderrBytes: 8_192,
});

const UUID_RE = /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/;
const CHANGE_ID_RE = /^[A-Za-z0-9][A-Za-z0-9._-]{0,127}$/;
const RESERVED_WINDOWS = new Set(['CON', 'PRN', 'AUX', 'NUL', 'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7', 'COM8', 'COM9', 'LPT1', 'LPT2', 'LPT3', 'LPT4', 'LPT5', 'LPT6', 'LPT7', 'LPT8', 'LPT9']);

function isMain() {
  return process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url);
}

export function utf8Bytes(value) {
  return Buffer.byteLength(String(value ?? ''), 'utf8');
}

export function truncateUtf8(value, maxBytes) {
  const text = String(value ?? '');
  if (utf8Bytes(text) <= maxBytes) return text;
  const marker = '\n...[truncated]';
  const markerBytes = utf8Bytes(marker);
  if (maxBytes <= markerBytes) return marker.slice(0, Math.max(0, maxBytes));
  let out = '';
  for (const char of text) {
    if (utf8Bytes(out + char) + markerBytes > maxBytes) break;
    out += char;
  }
  return out + marker;
}

function printError(message, details = {}) {
  console.error(truncateUtf8(JSON.stringify({ ok: false, gate: 'verifier-evidence', message, ...details }, null, 2), LIMITS.checkerStderrBytes));
}

function fail(message, details = {}) {
  printError(message, details);
  process.exit(1);
}

function parseArgs(argv = process.argv.slice(2)) {
  const args = {};
  for (let index = 0; index < argv.length; index += 1) {
    const token = argv[index];
    if (!token.startsWith('--')) continue;
    const eq = token.indexOf('=');
    if (eq >= 0) {
      args[token.slice(2, eq)] = token.slice(eq + 1);
      continue;
    }
    const key = token.slice(2);
    const next = argv[index + 1];
    if (next && !next.startsWith('--')) {
      args[key] = next;
      index += 1;
    } else {
      args[key] = true;
    }
  }
  return args;
}

function validateComponent(component, label) {
  if (typeof component !== 'string' || component.length === 0) throw new Error(`${label} is required`);
  if (component === '.' || component === '..') throw new Error(`${label} cannot be dot segment`);
  if (/[\\/:*?"<>|\x00-\x1F]/.test(component)) throw new Error(`${label} contains unsafe path character`);
  if (/[. ]$/.test(component)) throw new Error(`${label} cannot end in dot or space`);
  const base = component.split('.')[0].toUpperCase();
  if (RESERVED_WINDOWS.has(base)) throw new Error(`${label} uses reserved Windows name`);
}

export function normalizeExpected(input) {
  const changeId = String(input.changeId || '');
  const phase = String(input.phase || '');
  const invocationId = String(input.invocationId || '');
  const round = Number(input.round);
  if (!CHANGE_ID_RE.test(changeId)) throw new Error('changeId must be portable ASCII and non-empty');
  validateComponent(changeId, 'changeId');
  if (!['scoped', 'final'].includes(phase)) throw new Error('phase must be scoped or final');
  if (!Number.isSafeInteger(round) || round < 1) throw new Error('round must be a positive safe integer');
  if (!UUID_RE.test(invocationId)) throw new Error('invocationId must be canonical lowercase UUID');
  return { changeId, phase, round, invocationId };
}

export function evidencePaths(projectDir, expected) {
  const tuple = normalizeExpected(expected);
  const taskPackageDir = resolve(projectDir, '.claude', 'taskpackages', tuple.changeId);
  const taskPackagePath = resolve(taskPackageDir, 'taskpackage.json');
  const currentTaskPackagePath = resolve(projectDir, '.claude', 'taskpackages', 'current', 'taskpackage.json');
  const evidenceRoot = resolve(taskPackageDir, 'verifier-evidence');
  const roundsDir = resolve(evidenceRoot, 'rounds');
  const finalName = `r${tuple.round}.json`;
  validateComponent(finalName, 'finalName');
  const finalPath = resolve(roundsDir, finalName);
  const evidenceRef = `.claude/taskpackages/${tuple.changeId}/verifier-evidence/rounds/${finalName}`;
  return { ...tuple, taskPackageDir, taskPackagePath, currentTaskPackagePath, evidenceRoot, roundsDir, finalPath, finalName, evidenceRef };
}

function parseJsonTextNoDuplicates(text) {
  let index = 0;
  function error(message) { throw new Error(`${message} at byte ${index}`); }
  function skipWs() { while (/\s/.test(text[index] || '')) index += 1; }
  function parseStringToken() {
    const start = index;
    index += 1;
    while (index < text.length) {
      const ch = text[index];
      if (ch === '"') {
        index += 1;
        return JSON.parse(text.slice(start, index));
      }
      if (ch === '\\') index += 2;
      else index += 1;
    }
    error('unterminated string');
  }
  function parseNumberToken() {
    const match = /^-?(?:0|[1-9]\d*)(?:\.\d+)?(?:[eE][+-]?\d+)?/.exec(text.slice(index));
    if (!match) error('invalid number');
    index += match[0].length;
  }
  function parseLiteral(literal) {
    if (!text.startsWith(literal, index)) error(`expected ${literal}`);
    index += literal.length;
  }
  function parseArray() {
    index += 1;
    skipWs();
    if (text[index] === ']') { index += 1; return; }
    while (index < text.length) {
      parseValue();
      skipWs();
      if (text[index] === ']') { index += 1; return; }
      if (text[index] !== ',') error('expected comma in array');
      index += 1;
      skipWs();
    }
    error('unterminated array');
  }
  function parseObject() {
    const keys = new Set();
    index += 1;
    skipWs();
    if (text[index] === '}') { index += 1; return; }
    while (index < text.length) {
      if (text[index] !== '"') error('expected object key');
      const key = parseStringToken();
      if (keys.has(key)) throw new Error(`duplicate key ${key}`);
      keys.add(key);
      skipWs();
      if (text[index] !== ':') error('expected colon');
      index += 1;
      parseValue();
      skipWs();
      if (text[index] === '}') { index += 1; return; }
      if (text[index] !== ',') error('expected comma in object');
      index += 1;
      skipWs();
    }
    error('unterminated object');
  }
  function parseValue() {
    skipWs();
    const ch = text[index];
    if (ch === '{') return parseObject();
    if (ch === '[') return parseArray();
    if (ch === '"') return parseStringToken();
    if (ch === '-' || /\d/.test(ch || '')) return parseNumberToken();
    if (ch === 't') return parseLiteral('true');
    if (ch === 'f') return parseLiteral('false');
    if (ch === 'n') return parseLiteral('null');
    error('unexpected token');
  }
  parseValue();
  skipWs();
  if (index !== text.length) error('trailing content');
  return JSON.parse(text);
}

export function readJsonNoDuplicates(path, maxBytes = LIMITS.maxFactBytes) {
  const bytes = readFileSync(path);
  if (bytes.length >= 3 && bytes[0] === 0xef && bytes[1] === 0xbb && bytes[2] === 0xbf) throw new Error('UTF-8 BOM is not allowed');
  if (bytes.length > maxBytes) throw new Error(`JSON exceeds ${maxBytes} bytes`);
  return parseJsonTextNoDuplicates(bytes.toString('utf8'));
}

function componentParts(from, to) {
  const rel = relative(from, to);
  if (!rel || rel.startsWith('..') || isAbsolute(rel)) throw new Error(`path escapes root: ${to}`);
  return rel.split(sep).filter(Boolean);
}

export function assertSafeExistingComponents(projectDir, targetPath) {
  const logicalRoot = resolve(projectDir);
  const realRoot = realpathSync.native(logicalRoot);
  let cursor = logicalRoot;
  for (const part of componentParts(logicalRoot, resolve(targetPath))) {
    validateComponent(part, 'path component');
    cursor = resolve(cursor, part);
    if (!existsSync(cursor)) continue;
    const stat = lstatSync(cursor);
    if (stat.isSymbolicLink()) throw new Error(`reparse/symlink component rejected: ${cursor}`);
  }
  const parent = existsSync(targetPath) ? targetPath : dirname(targetPath);
  if (existsSync(parent)) {
    const realParent = realpathSync.native(parent);
    const rel = relative(realRoot, realParent);
    if (rel.startsWith('..') || isAbsolute(rel)) throw new Error('real path escapes project root');
  }
}

function readTaskPackage(path) {
  const text = readFileSync(path, 'utf8').replace(/^\uFEFF/, '');
  return JSON.parse(text);
}

export function validateTaskPackageIdentity(projectDir, expected, options = {}) {
  const paths = evidencePaths(projectDir, expected);
  if (!existsSync(paths.taskPackagePath)) throw new Error(`named TaskPackage missing: ${paths.taskPackagePath}`);
  const named = readTaskPackage(paths.taskPackagePath);
  if (named.changeId !== paths.changeId) throw new Error('named TaskPackage changeId mismatch');
  if (options.requireCurrent !== false) {
    if (!existsSync(paths.currentTaskPackagePath)) throw new Error('current TaskPackage missing');
    const current = readTaskPackage(paths.currentTaskPackagePath);
    if (current.changeId !== paths.changeId) throw new Error('current TaskPackage changeId mismatch');
  }
  assertSafeExistingComponents(projectDir, paths.taskPackageDir);
  assertSafeExistingComponents(projectDir, paths.evidenceRoot);
  return { paths, named };
}

function isCanonicalIsoMs(value) {
  return typeof value === 'string' && /^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z$/.test(value) && new Date(value).toISOString() === value;
}

function requireStringBytes(errors, value, label, max) {
  if (typeof value !== 'string') errors.push(`${label} must be a string`);
  else if (utf8Bytes(value) > max) errors.push(`${label} exceeds ${max} bytes`);
}

export function validateFactObject(fact, expectedInput, options = {}) {
  const errors = [];
  let expected;
  try { expected = normalizeExpected(expectedInput); } catch (error) { return { ok: false, errors: [error.message] }; }
  const allowed = new Set(['schemaVersion', 'artifactType', 'authority', 'producedBy', 'changeId', 'phase', 'round', 'invocationId', 'startedAt', 'completedAt', 'observedGateExit', 'ok', 'results', 'error']);
  for (const key of Object.keys(fact || {})) if (!allowed.has(key)) errors.push(`unknown field ${key}`);
  if (fact.schemaVersion !== '1.0') errors.push('schemaVersion must be 1.0');
  if (fact.artifactType !== 'verifier-round-fact') errors.push('artifactType mismatch');
  if (fact.authority !== 'bearing') errors.push('authority must be bearing');
  if (fact.producedBy !== 'verify.mjs') errors.push('producedBy mismatch');
  if (fact.changeId !== expected.changeId) errors.push('changeId mismatch');
  if (fact.phase !== expected.phase) errors.push('phase mismatch');
  if (fact.round !== expected.round) errors.push('round mismatch');
  if (fact.invocationId !== expected.invocationId) errors.push('invocationId mismatch');
  if (![0, 1].includes(fact.observedGateExit)) errors.push('observedGateExit must be 0 or 1');
  if (typeof fact.ok !== 'boolean') errors.push('ok must be boolean');
  if (!Array.isArray(fact.results)) errors.push('results must be array');
  if (!isCanonicalIsoMs(fact.startedAt)) errors.push('startedAt must be canonical UTC ISO milliseconds');
  if (!isCanonicalIsoMs(fact.completedAt)) errors.push('completedAt must be canonical UTC ISO milliseconds');
  if (isCanonicalIsoMs(fact.startedAt) && isCanonicalIsoMs(fact.completedAt)) {
    const started = Date.parse(fact.startedAt);
    const completed = Date.parse(fact.completedAt);
    if (started > completed) errors.push('startedAt must be <= completedAt');
    const now = options.now instanceof Date ? options.now.getTime() : Date.now();
    if (started > now + LIMITS.futureSkewMs || completed > now + LIMITS.futureSkewMs) errors.push('timestamp exceeds future skew tolerance');
  }
  const results = Array.isArray(fact.results) ? fact.results : [];
  if (results.length > LIMITS.maxResults) errors.push(`results exceeds ${LIMITS.maxResults}`);
  let stdoutBytes = 0;
  let stderrBytes = 0;
  let summaryBytes = 0;
  for (const [index, result] of results.entries()) {
    if (!result || typeof result !== 'object' || Array.isArray(result)) { errors.push(`results[${index}] must be object`); continue; }
    if (typeof result.ok !== 'boolean') errors.push(`results[${index}].ok must be boolean`);
    if (!Number.isInteger(result.exitCode) && !Number.isInteger(result.exit)) errors.push(`results[${index}] exit missing`);
    requireStringBytes(errors, result.gate || result.id, `results[${index}].gate`, 256);
    requireStringBytes(errors, result.stdout || '', `results[${index}].stdout`, LIMITS.perResultStdoutBytes);
    requireStringBytes(errors, result.stderr || '', `results[${index}].stderr`, LIMITS.perResultStderrBytes);
    requireStringBytes(errors, result.summary || '', `results[${index}].summary`, LIMITS.perResultSummaryBytes);
    stdoutBytes += utf8Bytes(result.stdout || '');
    stderrBytes += utf8Bytes(result.stderr || '');
    summaryBytes += utf8Bytes(result.summary || '');
  }
  if (stdoutBytes > LIMITS.aggregateStdoutBytes) errors.push('aggregate stdout exceeds limit');
  if (stderrBytes > LIMITS.aggregateStderrBytes) errors.push('aggregate stderr exceeds limit');
  if (summaryBytes > LIMITS.aggregateSummaryBytes) errors.push('aggregate summary exceeds limit');
  if (fact.ok === true) {
    if (fact.observedGateExit !== 0) errors.push('ok:true requires observedGateExit 0');
    if (fact.error !== null) errors.push('ok:true requires error:null');
    if (results.some((result) => result.ok !== true)) errors.push('ok:true requires all green results');
  } else if (fact.ok === false) {
    if (fact.observedGateExit !== 1) errors.push('ok:false requires observedGateExit 1');
    if (!fact.error || typeof fact.error !== 'object' || Array.isArray(fact.error)) errors.push('ok:false requires ErrorEnvelope');
    // 260813-gate-accuracy-batch T3/OQ-6=(i)：不变量放宽「ok:false ⇒ results 末条红」→「results 含 ≥1 红」——
    // final 相位全跑（ARQ-260805-final-stop-on-red-masking）后「前红后绿」混合形态为合法实况；
    // 仍拦「ok:false 全绿」矛盾态（诚实语义，FROZEN v1.0）。
    if (results.length === 0 || !results.some((result) => result.ok === false)) errors.push('ok:false requires at least one failed result');
  }
  if (fact.error && typeof fact.error === 'object') {
    requireStringBytes(errors, fact.error.name || fact.error.category || 'Error', 'error.name', LIMITS.errorNameBytes);
    requireStringBytes(errors, fact.error.message || '', 'error.message', LIMITS.errorMessageBytes);
    for (const [key, value] of Object.entries(fact.error)) {
      if (typeof value === 'string' && !['name', 'message'].includes(key)) requireStringBytes(errors, value, `error.${key}`, LIMITS.errorFieldBytes);
    }
    if (utf8Bytes(JSON.stringify(fact.error)) > LIMITS.errorEnvelopeBytes) errors.push('ErrorEnvelope exceeds limit');
  }
  return { ok: errors.length === 0, errors };
}

export function serializeFact(fact) {
  const text = JSON.stringify(fact, null, 2) + '\n';
  if (utf8Bytes(text) > LIMITS.maxFactBytes) throw new Error(`fact exceeds ${LIMITS.maxFactBytes} bytes`);
  parseJsonTextNoDuplicates(text);
  return text;
}

export function collectEvidenceState(projectDir, expected, options = {}) {
  const paths = evidencePaths(projectDir, expected);
  const errors = [];
  const finals = new Map();
  const temps = [];
  let aggregateBytes = 0;
  const now = options.now instanceof Date ? options.now : new Date();
  if (!existsSync(paths.roundsDir)) return { paths, errors, finals, temps, count: 0, aggregateBytes, nextRound: 1 };
  assertSafeExistingComponents(projectDir, paths.roundsDir);
  for (const entry of readdirSync(paths.roundsDir, { withFileTypes: true })) {
    const name = entry.name;
    const full = resolve(paths.roundsDir, name);
    if (entry.isSymbolicLink()) { errors.push(`reparse/symlink entry rejected: ${name}`); continue; }
    const finalMatch = /^r([1-9]\d*)\.json$/.exec(name);
    if (finalMatch) {
      const round = Number(finalMatch[1]);
      if (finals.has(round)) errors.push(`duplicate final round ${round}`);
      finals.set(round, full);
      aggregateBytes += statSync(full).size;
      continue;
    }
    const tempMatch = /^\.r([1-9]\d*)\.[A-Za-z0-9._-]+\.tmp$/.exec(name);
    if (tempMatch) {
      temps.push({ round: Number(tempMatch[1]), path: full, name, mtimeMs: statSync(full).mtimeMs });
      continue;
    }
    errors.push(`unknown evidence entry ${name}`);
  }
  const ordered = [...finals.keys()].sort((a, b) => a - b);
  for (let index = 0; index < ordered.length; index += 1) {
    if (ordered[index] !== index + 1) errors.push(`gap before round ${index + 1}`);
  }
  for (const round of ordered) {
    const path = finals.get(round);
    let fact;
    try {
      fact = readJsonNoDuplicates(path, LIMITS.maxFactBytes);
    } catch (error) {
      errors.push(`round ${round} final is malformed: ${error.message}`);
      continue;
    }
    const roundExpected = { changeId: paths.changeId, phase: fact && fact.phase, round, invocationId: fact && fact.invocationId };
    const validation = validateFactObject(fact, roundExpected, { now });
    if (!validation.ok) errors.push(`round ${round} final invalid: ${validation.errors.join('; ')}`);
  }
  for (const temp of temps) {
    if (!finals.has(temp.round)) errors.push(`temp without valid final for round ${temp.round}`);
    else if (options.cleanupTemps && Date.now() - temp.mtimeMs >= LIMITS.tempCleanupMs) {
      try { rmSync(temp.path, { force: true }); } catch { /* best effort */ }
    }
  }
  if (finals.size > LIMITS.maxFacts) errors.push('completed final count exceeds limit');
  if (aggregateBytes > LIMITS.maxAggregateBytes) errors.push('aggregate final bytes exceeds limit');
  return { paths, errors, finals, temps, count: finals.size, aggregateBytes, nextRound: finals.size + 1 };
}

export function preflightProducer(projectDir, expected) {
  validateTaskPackageIdentity(projectDir, expected);
  const state = collectEvidenceState(projectDir, expected, { cleanupTemps: true });
  if (state.errors.length) throw new Error(state.errors.join('; '));
  if (expected.round !== state.nextRound) throw new Error(`round ${expected.round} is not next completed ordinal ${state.nextRound}`);
  if (state.count >= LIMITS.maxFacts) throw new Error('active package final count is full');
  if (state.aggregateBytes > LIMITS.maxAggregateBytes - LIMITS.maxFactBytes) throw new Error('active package aggregate bytes exceed pre-gate admission boundary');
  mkdirSync(state.paths.roundsDir, { recursive: true });
  assertSafeExistingComponents(projectDir, state.paths.roundsDir);
  return state.paths;
}

export function validateEvidenceFile(projectDir, expectedInput, options = {}) {
  const expected = normalizeExpected(expectedInput);
  validateTaskPackageIdentity(projectDir, expected, { requireCurrent: options.requireCurrent !== false });
  const now = options.now instanceof Date ? options.now : new Date();
  const state = collectEvidenceState(projectDir, expected, { now });
  if (state.errors.length) return { ok: false, errors: state.errors };
  if (!existsSync(state.paths.finalPath)) return { ok: false, errors: ['expected final fact is missing'] };
  let fact;
  try { fact = readJsonNoDuplicates(state.paths.finalPath, LIMITS.maxFactBytes); }
  catch (error) { return { ok: false, errors: [error.message] }; }
  const validation = validateFactObject(fact, expected, { now });
  if (!validation.ok) return validation;
  return { ok: true, errors: [], fact, evidenceRef: state.paths.evidenceRef };
}

function writeSelfTaskPackage(root, changeId) {
  const named = resolve(root, '.claude', 'taskpackages', changeId);
  const current = resolve(root, '.claude', 'taskpackages', 'current');
  mkdirSync(named, { recursive: true });
  mkdirSync(current, { recursive: true });
  const taskPackage = { changeId, subsystem: 'self-check' };
  writeFileSync(resolve(named, 'taskpackage.json'), JSON.stringify(taskPackage, null, 2) + '\n');
  writeFileSync(resolve(current, 'taskpackage.json'), JSON.stringify(taskPackage, null, 2) + '\n');
}

function makeFact(expected, overrides = {}) {
  const now = new Date().toISOString();
  return {
    schemaVersion: '1.0',
    artifactType: 'verifier-round-fact',
    authority: 'bearing',
    producedBy: 'verify.mjs',
    changeId: expected.changeId,
    phase: expected.phase,
    round: expected.round,
    invocationId: expected.invocationId,
    startedAt: now,
    completedAt: now,
    observedGateExit: 0,
    ok: true,
    results: [{ id: 'self', gate: 'self', ok: true, exit: 0, exitCode: 0, stdout: '', stderr: '', summary: 'ok' }],
    error: null,
    ...overrides,
  };
}

function runSelfCheck() {
  const root = mkdtempSync(resolve(tmpdir(), 'verifier-evidence-'));
  const changeId = '260725-self-check';
  const invocationId = '11111111-1111-4111-8111-111111111111';
  const expected = { changeId, phase: 'scoped', round: 1, invocationId };
  try {
    writeSelfTaskPackage(root, changeId);
    const paths = evidencePaths(root, expected);
    mkdirSync(paths.roundsDir, { recursive: true });
    writeFileSync(paths.finalPath, serializeFact(makeFact(expected)));
    const checks = [];
    checks.push(['valid-green', validateEvidenceFile(root, expected).ok]);
    checks.push(['wrong-round', !validateEvidenceFile(root, { ...expected, round: 2 }).ok]);
    checks.push(['wrong-invocation', !validateEvidenceFile(root, { ...expected, invocationId: '22222222-2222-4222-8222-222222222222' }).ok]);
    writeFileSync(resolve(root, '.claude', 'taskpackages', 'current', 'verdict.json'), '{ malformed');
    checks.push(['projection-poison-ignored', validateEvidenceFile(root, expected).ok]);
    const failExpected = { ...expected, round: 2, invocationId: '33333333-3333-4333-8333-333333333333' };
    const failPaths = evidencePaths(root, failExpected);
    writeFileSync(failPaths.finalPath, serializeFact(makeFact(failExpected, {
      observedGateExit: 1,
      ok: false,
      results: [{ id: 'self', gate: 'self', ok: false, exit: 1, exitCode: 1, stdout: '', stderr: 'bad', summary: 'failed' }],
      error: { name: 'GateFailure', message: 'failed', gate: 'self', exitCode: 1 },
    })));
    checks.push(['valid-failure', validateEvidenceFile(root, failExpected).ok]);
    writeFileSync(failPaths.finalPath, '{"schemaVersion":"1.0","schemaVersion":"1.0"}\n');
    checks.push(['duplicate-key-rejected', !validateEvidenceFile(root, failExpected).ok]);
    const malformedPrior = { ...expected, round: 3, invocationId: '44444444-4444-4444-8444-444444444444' };
    const priorPaths = evidencePaths(root, { ...malformedPrior, round: 1, invocationId: expected.invocationId });
    writeFileSync(priorPaths.finalPath, '{ bad json\n');
    const laterPaths = evidencePaths(root, malformedPrior);
    writeFileSync(laterPaths.finalPath, serializeFact(makeFact(malformedPrior)));
    checks.push(['malformed-prior-final-fails', !validateEvidenceFile(root, malformedPrior).ok]);
    const pass = checks.filter(([, ok]) => ok).length;
    const fail = checks.length - pass;
    console.log(JSON.stringify({ selfCheck: 'verifier-evidence-check', pass, fail, total: checks.length, checks }, null, 2));
    process.exit(fail ? 1 : 0);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
}

function main() {
  if (process.argv.includes('--self-check')) runSelfCheck();
  const args = parseArgs();
  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  let expected;
  try {
    expected = normalizeExpected({ changeId: args['change-id'], phase: args.phase, round: args.round, invocationId: args['invocation-id'] });
  } catch (error) {
    fail(error.message);
  }
  let result;
  try {
    result = validateEvidenceFile(projectDir, expected);
  } catch (error) {
    fail('verifier evidence invalid', { errors: [error.message] });
  }
  if (!result.ok) fail('verifier evidence invalid', { errors: result.errors });
  const payload = { ok: true, gate: 'verifier-evidence', evidenceRef: result.evidenceRef, factOk: result.fact.ok, observedGateExit: result.fact.observedGateExit, phase: result.fact.phase, round: result.fact.round };
  console.log(truncateUtf8(JSON.stringify(payload), LIMITS.checkerStdoutBytes));
}

if (isMain()) main();
