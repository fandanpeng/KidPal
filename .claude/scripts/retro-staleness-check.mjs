#!/usr/bin/env node
// Retro staleness decision SSOT and advisory-only CLI.
// Importing this module has no output, filesystem scan, or process-exit side effect.

import assert from 'node:assert/strict';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { computeRetroScope } from './loop-health-report.mjs';

export const RETRO_STALE_DEFAULT_THRESHOLD = 10;

export function parseThreshold(envThreshold) {
  if (envThreshold === undefined || envThreshold === '') {
    return { threshold: RETRO_STALE_DEFAULT_THRESHOLD, thresholdSource: 'default', warnings: [] };
  }

  const parsed = Number(envThreshold);
  if (Number.isInteger(parsed) && parsed > 0) {
    return {
      threshold: parsed,
      thresholdSource: 'env DEVLOOP_RETRO_STALE_THRESHOLD',
      warnings: [],
    };
  }

  return {
    threshold: RETRO_STALE_DEFAULT_THRESHOLD,
    thresholdSource: 'default',
    warnings: [
      `DEVLOOP_RETRO_STALE_THRESHOLD="${envThreshold}" is not a positive integer; using default ${RETRO_STALE_DEFAULT_THRESHOLD}`,
    ],
  };
}

export function shouldFireAdvisory({ pendingCount, waitingCount, envThreshold }) {
  const thresholdResult = parseThreshold(envThreshold);
  const combined = pendingCount + waitingCount;
  return {
    wouldFire: combined > thresholdResult.threshold,
    combined,
    threshold: thresholdResult.threshold,
    thresholdSource: thresholdResult.thresholdSource,
    pendingCount,
    waitingCount,
    warnings: thresholdResult.warnings,
  };
}

function runSelfCheck() {
  const cases = [
    {
      label: 'below-default',
      input: { pendingCount: 5, waitingCount: 4, envThreshold: undefined },
      expected: { wouldFire: false, combined: 9, threshold: 10, thresholdSource: 'default', pendingCount: 5, waitingCount: 4, warnings: [] },
    },
    {
      label: 'equal-default',
      input: { pendingCount: 5, waitingCount: 5, envThreshold: undefined },
      expected: { wouldFire: false, combined: 10, threshold: 10, thresholdSource: 'default', pendingCount: 5, waitingCount: 5, warnings: [] },
    },
    {
      label: 'above-default',
      input: { pendingCount: 12, waitingCount: 29, envThreshold: undefined },
      expected: { wouldFire: true, combined: 41, threshold: 10, thresholdSource: 'default', pendingCount: 12, waitingCount: 29, warnings: [] },
    },
    {
      label: 'custom-integer',
      input: { pendingCount: 12, waitingCount: 29, envThreshold: '50' },
      expected: { wouldFire: false, combined: 41, threshold: 50, thresholdSource: 'env DEVLOOP_RETRO_STALE_THRESHOLD', pendingCount: 12, waitingCount: 29, warnings: [] },
    },
    {
      label: 'empty-is-unset',
      input: { pendingCount: 5, waitingCount: 5, envThreshold: '' },
      expected: { wouldFire: false, combined: 10, threshold: 10, thresholdSource: 'default', pendingCount: 5, waitingCount: 5, warnings: [] },
    },
    {
      label: 'padded-integer',
      input: { pendingCount: 5, waitingCount: 5, envThreshold: ' 10 ' },
      expected: { wouldFire: false, combined: 10, threshold: 10, thresholdSource: 'env DEVLOOP_RETRO_STALE_THRESHOLD', pendingCount: 5, waitingCount: 5, warnings: [] },
    },
  ];

  for (const envThreshold of ['abc', '3.7', '0', '-5', 'Infinity']) {
    const parsed = shouldFireAdvisory({ pendingCount: 12, waitingCount: 29, envThreshold });
    assert.equal(parsed.wouldFire, true, `invalid ${envThreshold}: wouldFire`);
    assert.equal(parsed.threshold, 10, `invalid ${envThreshold}: threshold`);
    assert.equal(parsed.thresholdSource, 'default', `invalid ${envThreshold}: source`);
    assert.equal(parsed.warnings.length, 1, `invalid ${envThreshold}: warning`);
  }

  for (const testCase of cases) {
    assert.deepEqual(shouldFireAdvisory(testCase.input), testCase.expected, testCase.label);
  }

  process.stdout.write(`${JSON.stringify({ selfCheck: 'retro-staleness', pass: cases.length + 5, fail: 0, total: cases.length + 5, ok: true })}\n`);
}

function isMain() {
  const invoked = process.argv[1];
  return Boolean(invoked && import.meta.url === pathToFileURL(resolve(invoked)).href);
}

function main() {
  if (process.argv.includes('--self-check')) {
    runSelfCheck();
    return;
  }

  const projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd();
  const scope = computeRetroScope(projectDir);
  const result = shouldFireAdvisory({
    pendingCount: scope.pendingAnalysis.length,
    waitingCount: scope.waitingOnLoopHealth.length,
    envThreshold: process.env.DEVLOOP_RETRO_STALE_THRESHOLD,
  });

  process.stdout.write(`${JSON.stringify({
    ...result,
    retroRuns: scope.retroRuns,
    alreadyAnalyzedCount: scope.alreadyAnalyzedCount,
  }, null, 2)}\n`);

  for (const warning of result.warnings) {
    process.stderr.write(`[retro-staleness] WARN: ${warning}\n`);
  }
  process.stderr.write(
    result.wouldFire
      ? `[retro-staleness] ADVISORY: combined=${result.combined} > threshold=${result.threshold}\n`
      : `[retro-staleness] OK: combined=${result.combined} <= threshold=${result.threshold}\n`,
  );
}

if (isMain()) main();