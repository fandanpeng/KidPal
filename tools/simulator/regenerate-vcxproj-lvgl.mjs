// regenerate-vcxproj-lvgl.mjs
//
// Owner: tools/simulator (dev-loop, TaskPackage 260831-simulator-lvgl-align)
// Purpose: 把 LvglWindowsSimulator.vcxproj(.filters) 中「lvgl 源文件清单」重新生成，
//          与当前 LvglPlatform/lvgl/ 实树保持一致。用于 LVGL 版本降级/升级后同步文件引用。
//          （上游 lv_port_pc_visual_studio 自带 C# 版 LvglProjectFileUpdater，但其仓库根解析
//           不适用本仓 tools/simulator 嵌套布局，故用同语义的 node 版替代。）
// 语义: 与上游 UpdateLvglWindowsSimulator 一致——枚举 LvglPlatform/lvgl 全树，
//       .h→ClInclude / .c→ClCompile(/utf-8) / 其它→None；
//       .devcontainer/.github/docs/tests/env_support/scripts 子路径强制 None。
// 用法: node tools/simulator/regenerate-vcxproj-lvgl.mjs
import { readFileSync, writeFileSync, readdirSync, statSync } from 'fs';
import { join, dirname, relative } from 'path';
import { fileURLToPath } from 'url';
import { randomUUID } from 'crypto';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const PLATFORM = join(SCRIPT_DIR, 'LvglPlatform');
const LVGL_ROOT = join(PLATFORM, 'lvgl');
const VCPROJ = join(SCRIPT_DIR, 'LvglWindowsSimulator', 'LvglWindowsSimulator.vcxproj');
const FILTERS = join(SCRIPT_DIR, 'LvglWindowsSimulator', 'LvglWindowsSimulator.vcxproj.filters');

const HEADER_EXTS = new Set(['.h', '.hh', '.hpp', '.hxx', '.h++', '.hm', '.inl', '.inc', '.ipp']);
const SOURCE_EXTS = new Set(['.cpp', '.c', '.cc', '.cxx', '.c++', '.cppm', '.ixx']);
const FORCE_OTHERS = ['.devcontainer', '.github', 'docs', 'tests', 'env_support', 'scripts'];

function walk(dir) {
  const out = [];
  for (const name of readdirSync(dir).sort()) {
    const full = join(dir, name);
    if (statSync(full).isDirectory()) out.push(...walk(full));
    else out.push(full);
  }
  return out;
}

function extOf(full) {
  const i = full.lastIndexOf('.');
  return i < 0 ? '' : full.slice(i).toLowerCase();
}

function classify(full) {
  const rel = relative(PLATFORM, full).replaceAll('/', '\\');
  if (FORCE_OTHERS.some((s) => rel.includes(s))) return { rel, kind: 'none' };
  const e = extOf(full);
  if (HEADER_EXTS.has(e)) return { rel, kind: 'include' };
  if (SOURCE_EXTS.has(e)) return { rel, kind: 'compile' };
  return { rel, kind: 'none' };
}

const files = walk(LVGL_ROOT).map(classify).sort((a, b) => a.rel.localeCompare(b.rel));

const MSBASE = '$(MSBuildThisFileDirectory)..\\LvglPlatform\\';
const inc = (rel) => `${MSBASE}${rel}`;

const headers = files.filter((f) => f.kind === 'include');
const sources = files.filter((f) => f.kind === 'compile');
const others = files.filter((f) => f.kind === 'none');
const dirs = [...new Set(files.map((f) => dirname(f.rel)))].sort();

function vcxprojItem(f) {
  if (f.kind === 'include') return `    <ClInclude Include="${inc(f.rel)}" />`;
  if (f.kind === 'compile') {
    return [
      `    <ClCompile Include="${inc(f.rel)}">`,
      '      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>',
      '      <LanguageStandard>Default</LanguageStandard>',
      '    </ClCompile>',
    ].join('\r\n');
  }
  return `    <None Include="${inc(f.rel)}" />`;
}

function filtersItem(f) {
  const tag = f.kind === 'compile' ? 'ClCompile' : f.kind === 'include' ? 'ClInclude' : 'None';
  return [
    `    <${tag} Include="${inc(f.rel)}">`,
    `      <Filter>${dirname(f.rel)}</Filter>`,
    `    </${tag}>`,
  ].join('\r\n');
}

function filtersDir(d) {
  return [
    `    <Filter Include="${d}">`,
    `      <UniqueIdentifier>{${randomUUID().toUpperCase()}}</UniqueIdentifier>`,
    '    </Filter>',
  ].join('\r\n');
}

function read(p) {
  const buf = readFileSync(p);
  const hasBom = buf.length >= 3 && buf[0] === 0xef && buf[1] === 0xbb && buf[2] === 0xbf;
  return { text: hasBom ? buf.toString('utf8').slice(1) : buf.toString('utf8'), hasBom };
}
function write(p, hasBom, text) {
  writeFileSync(p, (hasBom ? '﻿' : '') + text, 'utf8');
}

// ── vcxproj ──
function stripVcxprojLvgl(t) {
  t = t.replace(
    /[\t ]*<(?:ClInclude|None) Include="\$\(MSBuildThisFileDirectory\)\.\.\\LvglPlatform\\lvgl\\[^"]*" \/>\r?\n?/g,
    '',
  );
  t = t.replace(
    /[\t ]*<ClCompile Include="\$\(MSBuildThisFileDirectory\)\.\.\\LvglPlatform\\lvgl\\[^"]*">\r?\n[\t ]*<AdditionalOptions>[^<]*<\/AdditionalOptions>\r?\n[\t ]*<LanguageStandard>[^<]*<\/LanguageStandard>\r?\n[\t ]*<\/ClCompile>\r?\n?/g,
    '',
  );
  return t;
}

// ── filters ──
function stripFiltersLvgl(t) {
  t = t.replace(
    /[\t ]*<(?:ClInclude|ClCompile|None) Include="\$\(MSBuildThisFileDirectory\)\.\.\\LvglPlatform\\lvgl\\[^"]*">\r?\n[\t ]*<Filter>[^<]*<\/Filter>\r?\n[\t ]*<\/(?:ClInclude|ClCompile|None)>\r?\n?/g,
    '',
  );
  t = t.replace(
    /[\t ]*<Filter Include="lvgl(?:\\[^"]*)">\r?\n[\t ]*<UniqueIdentifier>[^<]*<\/UniqueIdentifier>\r?\n[\t ]*<\/Filter>\r?\n?/g,
    '',
  );
  return t;
}

function insertBefore(t, anchor, chunk) {
  if (!t.includes(anchor)) throw new Error('anchor not found: ' + anchor);
  return t.replace(anchor, chunk + anchor);
}

const H = headers.map(vcxprojItem).join('\r\n') + '\r\n';
const S = sources.map(vcxprojItem).join('\r\n') + '\r\n';
const O = others.map(vcxprojItem).join('\r\n') + '\r\n';

let m = read(VCPROJ);
let t = stripVcxprojLvgl(m.text);
t = insertBefore(t, '    <ClInclude Include="lv_conf.h" />', H);
t = insertBefore(t, '    <ClCompile Include="LvglWindowsSimulator.cpp" />', S);
t = insertBefore(t, '    <None Include="freetype.props" />', O);
write(VCPROJ, m.hasBom, t);

const FH = headers.map(filtersItem).join('\r\n') + '\r\n';
const FS = sources.map(filtersItem).join('\r\n') + '\r\n';
const FO = others.map(filtersItem).join('\r\n') + '\r\n';
const FD = dirs.map(filtersDir).join('\r\n') + '\r\n';

let n = read(FILTERS);
let u = stripFiltersLvgl(n.text);
u = insertBefore(u, '    <ClInclude Include="lv_conf.h" />', FH);
u = insertBefore(u, '    <ClCompile Include="LvglWindowsSimulator.cpp" />', FS);
u = insertBefore(u, '    <None Include="$(MSBuildThisFileDirectory)..\\LvglPlatform\\freetype\\.clang-format">', FO);
u = insertBefore(u, '    <Filter Include="freetype">', FD);
write(FILTERS, n.hasBom, u);

console.log('regenerated. headers=%d sources=%d none=%d dirs=%d', headers.length, sources.length, others.length, dirs.length);