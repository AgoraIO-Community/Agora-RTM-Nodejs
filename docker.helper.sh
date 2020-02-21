# delete build/
sudo rm -rf build
# rebuild image
sudo docker build -t agora/rtm .
# kill old container
sudo docker kill $(docker ps -a -q --filter="name=rtminstance")
sudo docker rm $(docker ps -a -q --filter="name=rtminstance")
# run new contaienr 
sudo docker run  -e AGORA_APP_ID=$AGORA_APP_ID  --name rtminstance agora/rtm 
# copy build/ to outside
sudo docker cp $(docker ps -a -q --filter="name=rtminstance"):/usr/src/app/build ./build
