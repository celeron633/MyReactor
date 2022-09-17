# MyReactor

## 项目介绍

* 个人练习项目，模仿muduo实现的基于epoll的网络编程库，用于学习epoll和reactor模式服务器编程，epoll使用LT模式

## 开发环境

* linux kernel version 4.15.0 (ubuntu 20.04 desktop)
* gcc version 7.5.0
* cmake version 3.10.2

## 编译指导
```shell
git clone ${project_url}
cd MyReactor
mkdir -p build
cd build
cmake ..
make
```

## 性能测试