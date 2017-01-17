# #Define an array and visit everyone

# array=(
# "astar"
# "bzip2"
# "bwaves"
# "gcc"
# "mcf"
# "zeusmp"
# "cactus"
# "gemsfdtd"
# "lbm"
# )
# len=${#array[@]}
# for((i=0;i<$len;i++))
# do
# 	echo i=$i ${array[$i]}
# done
# *********************************************************************************
array=(
"astar"
"bzip2"
"bwaves"
"gcc"
"mcf"
"zeusmp"
"cactus"
"gemsfdtd"
"lbm"
)
len=${#array[@]}
for((i=0;i<$len;i++))
do
	echo "runing" i=$i ${array[$i]}
	./getHisgraph.sh ${array[$i]}
done
echo "ls"
