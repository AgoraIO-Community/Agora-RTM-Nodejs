# delete build/
rm -rf build
# rebuild image
docker build -t agora/rtm .
# kill old container
# docker kill $(docker ps -a -q --filter="name=rtminstance")
# run new contaienr 
docker run --name rtminstance -d agora/rtm 
# copy build/ to outside
docker cp $(docker ps -a -q --filter="name=rtminstance"):/usr/src/app/build ./build