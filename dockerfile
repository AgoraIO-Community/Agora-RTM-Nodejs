FROM node:8

# Create app directory
WORKDIR /usr/src/app

RUN apt-get update && apt-get install -y \
  uuid-dev \
  libevent-dev
# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
# where available (npm@5+)
COPY package*.json ./

COPY libagora_rtm_sdk.so /usr/lib/

RUN npm install --ignore-scripts
# If you are building your code for production
# RUN npm ci --only=production
# Bundle app source
COPY . .

RUN npm run build:addon

RUN npm run build:ts

RUN npm run test

# EXPOSE 8080

# CMD [ "/bin/ls", "./build" ]