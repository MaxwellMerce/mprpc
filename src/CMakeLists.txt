#由于CMake生成make文件时一次性的，在Cmakelists不变的情况下不会生成第二遍，所以当我们后面新加了文件，就可能导致哪些新加的文件没
#参与编译引发bug，对于这种情况建议删除掉Make文件重新生成
aux_source_directory(. SRC_lIST)
#将src下的文件编译成库
add_library(mprpc ${SRC_lIST})
#设置依赖
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
