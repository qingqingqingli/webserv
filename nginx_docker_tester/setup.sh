# build the image
docker build -t nginx_docker .

# run docker image & expose ports
docker run -it -p 80:80 nginx_docker

# remove all stopped containers
# docker rm $(docker ps --filter status=exited -q)

# remove all docker images
# docker rmi $(docker images -a -q)

# check running docker containers
# docker ps

# check stopped docker containers
# docker ps -a

# check docker images
# docker images

# remove a specific container
# docker rm xxx

# remove a specific image
# docker rmi xxx