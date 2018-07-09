# 代码编译运行指南

1. 使用CmakeGUI创建项目

   注意点击configure选择对应VS版本

![](./readmeimg/1.png)



2. 将cmake项目里面res文件夹内所有内容

   如：

   ![](./readmeimg/2.png)

   拖到新创建的VS项目根目录下，如：

   ![](./readmeimg/3.png)

3. 将Cmake项目根目录下的assimpd.dll

   如：

   ![](./readmeimg/4.png)

   复制到新建VS项目的DEBUG目录下

4. 打开VS项目

   右键ALL_BUILD,点击Build

   ![](./readmeimg/5.png)



​	右键CG_PRO点击Set as StartUp project

![](./readmeimg/6.png)

​	然后就可以直接运行项目了





​	**最终项目结构如下：**

​	![](./readmeimg/7.png)