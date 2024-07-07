# 1、概述
此仓库主要用于记录Linux学习过程的笔记中程序代码；

# 2、git 账户配置
```
git config --global user.email "xxx.com"
git config --global user.name  "xxx"

# git config命令的–global参数，用了这个参数，表示你这台机器上所有的Git仓库都会使用这个配置，当然也可以对某个仓库指定不同的用户名和Email地址。

# 1.查看git配置信息
$ git config --list

# 2.查看git用户名、密码、邮箱的配置
$ git config user.name
$ git config user.password
$ git config user.email

# 3.设置git用户名、密码、邮箱的配置

$ git config user.name "freedom"
$ git config user.password "123456"
$ git config user.email "1548429568@qq.com"
# 3.设置git用户名、密码、邮箱的配置（全局配置）
$ git config --global user.name 用户命
$ git config --global user.name freedom
$ git config --global user.password 密码
$ git config --global user.password abc0506abc
$ git config --global user.password 邮箱
$ git config --global user.email "1548429568@qq.com"

# 4.修改git用户名、密码、邮箱的配置（跟设置语法一样，没有用户名就添加，有了用户名就修改）
$ git config user.name "freedom"
# 4.修改git用户名、密码、邮箱的配置（全局配置）
$ git config --global user.name "freedom"

# 设置全局编辑器为vim
git config --global core.editor "vim"
```
# 3、配置ssh
```
ssh-keygen -t rsa -C "www.xxx.com"
```
# 4、增加git commit 模板
```
git config --global commit.template /home/xxx/commit.template

```
