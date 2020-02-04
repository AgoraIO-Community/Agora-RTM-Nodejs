# delete build/
sudo rm -rf build
# rebuild image
sudo docker build -t agora/rtm .
# kill old container
# docker kill $(docker ps -a -q --filter="name=rtminstance")
# run new contaienr 
sudo docker run --name rtminstance -d agora/rtm 
# copy build/ to outside
sudo docker cp $(docker ps -a -q --filter="name=rtminstance"):/usr/src/app/build ./build
