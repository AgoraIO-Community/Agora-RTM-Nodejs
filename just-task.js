const { task, option, logger, argv, series, condition } = require('just-task');
const synclib = require('./scripts/synclib')
const {getArgvFromNpmEnv, getArgvFromPkgJson} = require('./scripts/npm_argv')

option('debug', {default: false, boolean: true});
option('silent', {default: false, boolean: true});
option('liburl', {default: ''});

const packageVersion = require('./package.json').version;

task('sync:lib', () => {
  const config = Object.assign({}, getArgvFromPkgJson(), getArgvFromNpmEnv() )
  return synclib({
    platform: argv().platform,
    // platform: 'win32',
    liburl: argv().liburl || config.liburl
  })
})
