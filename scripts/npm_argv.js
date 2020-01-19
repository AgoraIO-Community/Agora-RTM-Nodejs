const path = require('path')

module.exports.getArgvFromNpmEnv = () => {
  return {
    debug: process.env.npm_config_agora_electron_debug === 'true',
    silent: process.env.npm_config_agora_electron_silent === 'true'
  }
}

module.exports.getArgvFromPkgJson = () => {
  const projectDir = path.join(process.env.INIT_CWD, 'package.json')
  const pkgMeta = require(projectDir);
  if (pkgMeta.agora) {
    const liburl = pkgMeta.agora.liburl || ""
    return {
      debug: pkgMeta.agora.debug === true,
      silent: pkgMeta.agora.silent === true,
      liburl
    }
  } else {
    return {
      prebuilt: true
    }
  }
}