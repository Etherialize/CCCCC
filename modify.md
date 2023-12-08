#### 中端

* 修改Function、BasicBlock、Module、ConstVal继承Value类       **OK**
* Value中增加parent                                          **OK**
* 修改了中端不合理的参数Alloca和后端           **OK**
* 增加了Br的前继和后继信息                             **OK**
* 删去没有前继的basicBlock（除entry），计算CFG图     **OK**
  * 修改了计算支配者时初始化的问题

* 将alloca提到函数入口                                         **OK**
* 将Value构造函数统一到irBuilder类中，将value中用到的id也都放到irBuilder中。修改过程中造成了以下bug，太蠢了吧
* MemToReg                                            **OK**
  * 中端加入Phi
  * 消Phi                                                     **OK**


![image-20231205114532669](../../AppData/Roaming/Typora/typora-user-images/image-20231205114532669.png)