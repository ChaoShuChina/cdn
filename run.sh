#!/usr/bin/env bash

#N[${#N[@]}]="medium/case0"
#N[${#N[@]}]="medium/case1"
#N[${#N[@]}]="medium/case2"
#N[${#N[@]}]="medium/case3"
#N[${#N[@]}]="medium/case4"
#N[${#N[@]}]="medium/case5"
#N[${#N[@]}]="medium/case6"
#N[${#N[@]}]="medium/case7"
#N[${#N[@]}]="medium/case8"
#N[${#N[@]}]="medium/case9"

#N[${#N[@]}]="advanced/case0"
#N[${#N[@]}]="advanced/case1"
#N[${#N[@]}]="advanced/case2"
#N[${#N[@]}]="advanced/case3"
#N[${#N[@]}]="advanced/case4"
#N[${#N[@]}]="advanced/case5"
#N[${#N[@]}]="advanced/case6"
#N[${#N[@]}]="advanced/case7"
#N[${#N[@]}]="advanced/case8"
#N[${#N[@]}]="advanced/case9"

#cp cdn.cpp cdn.h importOpt.cpp ../SDK-gcc-upload/cdn/    # 自己测试用
cp ../cdn.cpp ../SDK-gcc-upload/cdn/          # 正式上传用
rm ../SDK-gcc-upload/cdn/importOpt.cpp ../SDK-gcc-upload/cdn/cdn.h

cp  include.h getPaths.cpp readGraph.cpp MCFSimplex.cpp MCFSimplex.h ../SDK-gcc-upload/cdn/
cp deploy.cpp deploy.h Node.h Tree.cpp Tree.h ../SDK-gcc-upload/cdn/
dir="/Users/xuxu/Desktop/CodeCraft2017/cdn/case/"
cd ../SDK-gcc-upload/
if [ -f cdn.tar.gz ]
then
    rm cdn.tar.gz
fi
./build.sh
cp bin/cdn ../cdn/cdn
cd ../cdn


> runout.txt
length=${#N[@]}
for ((i=0; i<$length; i++))
do
	case=${N[$i]}
	echo "start :"${case} >> runout.txt
	caseDir=${dir}${case}
	input=${caseDir}".txt"
	output=${caseDir}"_result.txt"
	for ((j=0; j<1; j++))
	do
		./cdn ${input} ${output} >> runout.txt
	done
	echo "done  "${case}
	echo -e "\n " >> runout.txt
done