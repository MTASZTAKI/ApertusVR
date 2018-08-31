import _url from 'url';
const builtins = new Set(
  Object.keys(process.binding('natives')).filter(str =>
    /^(?!(?:internal|node|v8)\/)/.test(str))
)

const baseURL = new _url.URL('file://');
baseURL.pathname = process.cwd() + '/';

export function resolve (specifier, base = baseURL) {
  if (builtins.has(specifier)) {
    return {
      url: specifier,
      format: 'builtin'
    };
  }
  // load all dependencies as esm, regardless of file extension
  const url = new _url.URL(specifier, base).href;
  return {
    url,
    format: 'esm'
  };
}
