# Michelle 的学生会工作

[评测地址](https://acm.sjtu.edu.cn/OnlineJudge/problem?problem_id=1346)

## 1 背景

Michelle 接到了花咲川学生会的协助请求，想要她帮忙写一个程序管理学生的信息。但她还有 `Hello, Happy World` 新曲的编曲没有完成，实在是没有时间了。你能帮帮她吗？

## 2 题目目的

本题目为 ICPC Management System 的前置作业，意在确保大家掌握结构体与部分 STL 容器的基本操作。

## 3 任务描述

在这个任务中，你需要实现一个管理系统，通过控制台输入对学生信息进行维护。

题目保证：

* 输入格式合法
* 在开始统计前只会出现 `ADD` 操作
* `START` 操作仅出现一次

具体需要实现以下操作。

### 添加学生

```
ADD <name> <gender> <class> <score0> ... <score8>
```

向系统中添加一名学生。学生有 12 个参数，`name`、`gender`、`class` 与 9 个成绩。

* `name`为长度不超过 30 的字符串，由数字、大小写字母与下划线组成；
* `gender` 为一个字母，`M` 代表男性，`F` 代表女性；
* `class` 为 $[1,20]$ 内的一个整数；
* `scorei` 为课程代号为 i（i = 0, 1, ..., 8）的课程的得分，得分为 $[1,100]$ 内的一个整数。

开始统计后不能再添加学生。若已经开始统计，输出 `[Error]Cannot add student now.`；若未开始统计但学生名字重复输出 `[Error]Add failed.`。

本题中，添加失败只有一种情况，即学生名字重复。

### 开始统计

```
START
```

开始进行学生的成绩统计。在开始统计时，榜单应该按照顺序排列。

### 更新成绩

```
UPDATE <name> <code> <score>
```

更新学生 `name` 课程代号为 `code` 的分数为 `score`。保证 `score` 为 $[0,100]$ 内的整数，`code` 为 $[0,8]$ 内的整数。

若学生不存在输出 `[Error]Update failed.`。

### 刷新榜单

```
FLUSH
```

刷新榜单。**注意更改学生成绩但未刷新榜单时榜单的排名不会变化**（但数据可能变化）。

### 输出榜单

```
PRINTLIST 
```

输出共 N（学生总数）行，每行以

```
<rank> <name> <gender> <class> <avg_score>
```

的格式输出，其中 `rank` 为 `1-based`，`gender` 男性输出`male`，女性输出 `female`；`avg_score` 为该学生所有分数的平均值，向下取整。

学生的排名由平均成绩（即 `avg_score`，注意为向下取整的平均成绩）决定，平均成绩高者靠前；平均成绩相同时从 0 到 8 分别比较成绩，成绩高者靠前；依旧平局时，比较学生名字的字典序，字典序较小者靠前。

### 查询学生排名

```
QUERY <name>
```

若学生存在，以

```
STUDENT <name> NOW AT RANKING <ranking>
```

的格式输出学生的排名，1-based。否则，输出 `[Error]Query failed.`。

### 退出程序

```
END
```

结束程序。

## 4 数据范围

学生总数为 $n$ ，操作次数为 $\mathit{opt}$ 。

对于 60% 数据， $n\le 100$ ， $\mathit{opt}\le 3\times 10^3$ 。​​

对于 100% 数据， $n\le 10^4$ ， $\mathit{opt}\le 3\times 10^5$ ​。​

## 5 提示

在这道题目中，你应该使用一个 `struct` 去保存所有学生的信息，并使用一个 `string` 到该结构体的 `map` 来维护学生的信息；同时，你需要使用 `set` 维护一个排行榜，排序标准为学生的成绩。

`map` 与 `set` 的相关接口可以在 [cppreference](https://en.cppreference.com/w/) 上学习。

本目录下的 `data` 文件夹即 Online Judge 上的测试数据点。

在 `START` 之后，各个指令出现的频率大致如下：

* `UPDATE` 45%
* `QUERY` 45%
* `ADD` 5%
* `FLUSH` 4.9%
* `PRINTLIST` 0.1%

如果你遇到 `Time Limit Exceed` 的问题，可以考虑以下几种优化方法。

### 语言相关优化

* 将 `map` 换成 `unordered_map`。这两个容器在接口上十分相似，且均实现了映射的功能，但 `unordered_map` 的性能较好。（相应的，`unordered_map` 牺牲了容器内元素的有序性。）

```C++
#include <string>
#include <unordered_map> // 需要头文件哦

/**
 * 下面这个 nameMap 维护了从 string 到 long long 的映射
 * 
 * 需要注意的是，如果是 map ，那么用迭代器遍历这个容器是
 * 按照 key 类型 (本例中即为 string) 从小到大地遍历每个元素。
 * 
 * 如果是 unordered_map ，其不具有这种有序性，仅仅保证能遍历全部元素。。
*/
std::unordered_map <std::string,long long> nameMap;
```

* 对于非内置类型 (特别是 STL 的容器)，避免使用函数的值传递，改成 `const &` 传递。

```C++
#include <string>

/**
 * 以下面这个函数为例。
 * 
 * 对于 vector/string/set/map 等非内置类型 (包括你自己的结构体)，
 * 在传递参数的时候，尽可能使用引用来传递参数 (即在类型后面添加 & 符号)。
 * 
 * 如果不需要在函数内修改这个参数，那么请在类型前面添加一个 const。
 * 例如下面这个函数便用的是 const std::string & 来传递参数。
 * 
 * 对于 bool/char/int/long long 这些基本类型，则不需要用引用
 * 传递参数，只需要正常的值传递参数即可，如下面这个函数。
 * 
 * 如果你感兴趣的话，可以自行查阅引用传参和值传参的不同之处。
 * 以上仅仅是一种非常粗糙但管用的分类方法。
*/
void func0(const std::string & str,int x);

// 尽量不要这么写，除非你清楚你在做什么
void func1(std::string str,const int & x);

```

* 你是否在输出的时候使用 `std::endl` 来换行？如果仅仅是为了输出换行，请把 `std::endl` 换成字符 `\n` 。[参考解释](https://stackoverflow.com/questions/213907/stdendl-vs-n)。

```C++
#include <iostream>

/**
 * 这个函数会输出:
 * 
 * Hello World!
 * 42
 * 
 * 事实上，std::endl 经常被误用于仅仅需要换行的地方，
 * 网上大量的资料，甚至教科书都会错误的使用 std::endl ！
 * 
 * std::endl 除了换行，还会导致 std::ostream 的输出缓冲区被刷新 (flush) ，
 * 频繁使用带来的后果就是糟糕缓冲性能 (几乎相当于没有缓冲，甚至可能更糟)。
 * 
 * 一个例外是如果你的程序在 debug 阶段，随时可能出现异常，
 * 那么 std::endl 可以保证你的程序的输出可以实时地输出，而不会因为异常导致未输出完。
 * 
*/
void printTest() {
    int x = 42;
    std::cout << "Hello World!\n";
    std::cout << x << '\n';
}

```




* 对于 STL 的容器，部分成员函数会返回一些参数，这些参数可能会有用。具体参数以及含义，请以 [cppreference](https://en.cppreference.com/w/) 为主。




### 题目相关优化

* 你是否在 `map` 和 `set` 中各保存了一份学生信息？对于冗余信息的大量移动与复制是十分消耗时间的。可以在 `set` 中只保存少量元素，例如只保存学生名字，或者使用一个数组或者 `vector` 来存放学生的信息，`map` 和 `set` 都只存放学生在 `vector` 中的位置。

* 你是否每次 QUERY 指令都要从头遍历一遍 `set`？这个操作的时间复杂度是 $O(n)$ 的，对于占比 45% 的 QUERY 指令来说是一个不可接受的复杂度。可以尝试把这个排名存下来，输出的时候直接去调用。

* 对于 FLUSH 指令，你是否在执行时将 `set` 清空并全部重新插入？你可以试试在 UPDATE 时就维护正确的排名。注意，当 `set` 中的元素值变化后，其在容器中的位置 **不会** 及时更新；你可以先按照这个元素旧的值 erase ，然后把更新后的元素值重新 insert 。
