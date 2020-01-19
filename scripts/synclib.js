const { logger } = require("just-task");
const download = require("download");
const extract = require('extract-zip')
const path = require("path");
const glob = require("glob")
const promisify = require("bluebird").promisify
const fs = require("fs-extra")

const extractPromise = promisify(extract)
const globPromise = promisify(glob)


const prepare = () => {
  return new Promise((resolve, reject) => {
    Promise.all([
      fs.remove(path.join(__dirname, '../sdk'))
    ]).then(() => {
      return fs.move(
        path.join(__dirname, '../tmp/Agora_RTM_SDK_for_Linux/libs'),
        path.join(__dirname, '../sdk/')
      )
    }).then(() => {
      resolve()
    }).catch(e => {
      reject(e)
    })
  })
}


module.exports = ({
  liburl
}) => {

  return new Promise((resolve, reject) => {
    let downloadUrl;
    if(!liburl){
      logger.error(`no lib specified`)
      return reject(new Error(`no lib specified`))
    } else {
      downloadUrl = liburl
    }

    /** start download */
    const outputDir = "./tmp/";
    logger.info(`Downloading Libs...\n${downloadUrl}\n`);

    fs.remove(path.join(__dirname, '../tmp')).then(() => {
      return download(downloadUrl, outputDir, {filename: "sdk.zip"})
    }).then(() => {
      logger.info("Success", "Download finished");
      return extractPromise('./tmp/sdk.zip', {dir: path.join(__dirname, '../tmp/')})
    }).then(() => {
        logger.info("Success", "Unzip finished");
        return globPromise(path.join(__dirname, '../tmp/Agora_RTM_SDK_for_Linux*/'))
    }).then(folder => {
      return prepare()
    }).then(() => {
      logger.info("Success", "Prepare finished");
      resolve()
    }).catch(err => {
      logger.error("Failed: ", err);
      reject(new Error(err));
    });
  })
};