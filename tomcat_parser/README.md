# Tomcat Access Log Parser

## Compile

```bash
make test
make debug # debug mode
```

## Run

```bash
test/basic
test/stress # performance testing with randomly generated logs
```

## Result

目前只测试了tomcat access log默认的两种日志格式`%h %l %u %t "%r" %s %b`、`%h %l %u "%r" %s %b "%{Referer}i" "%{User-Agent}i"`。

Parser是我优化过的日志解析器，而RegexParser是使用单个带有捕获组的正则表达式的日志解析器，也是大部分通用日志解析器的做法，作为baseline。

```bash
$ test/basic
Parser process:
Parser used 161678 ns

host: 192.168.1.1
logicalUserName: -
user: user2
time: 11/Sep/2023:12:00:00 +0000
request: POST /page2
statusCode: 200
size: 2048

host: 10.0.0.1
logicalUserName: -
user: user1
time: 11/Sep/2023:11:00:00 +0000
request: DELETE /page2
statusCode: 404
size: 1024

host: 172.16.0.1
logicalUserName: -
user: -
time: 11/Sep/2023:10:00:00 +0000
request: PUT /page3
statusCode: 404
size: 4096

host: 192.168.1.1
logicalUserName: -
user: user1
time: 17/Sep/2023:10:00:00 +0000
request: GET /page1
statusCode: 500
size: 2048

host: 10.0.0.1
logicalUserName: -
user: -
time: 17/Sep/2023:11:00:00 +0000
request: POST /page3
statusCode: 200
size: 1024

Regex parser process:
Regex parser used 157940 ns

Parser process:
Parser used 156668 ns

host: 192.168.1.1
logicalUserName: -
user: user1
request: GET /page2
statusCode: 404
size: 2048
requestHeaders-Referer: http://example.com/page1
requestHeaders-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36

host: 10.0.0.1
logicalUserName: -
user: -
request: POST /page1
statusCode: 200
size: 1024
requestHeaders-Referer: http://example.com/page3
requestHeaders-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36

host: 172.16.0.1
logicalUserName: -
user: user2
request: PUT /page3
statusCode: 200
size: 4096
requestHeaders-Referer: http://example.com/page2
requestHeaders-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36

host: 192.168.1.1
logicalUserName: -
user: -
request: DELETE /page2
statusCode: 404
size: 2048
requestHeaders-Referer: http://example.com/page1
requestHeaders-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36

host: 10.0.0.1
logicalUserName: -
user: -
request: GET /page1
statusCode: 500
size: 1024
requestHeaders-Referer: http://example.com/page2
requestHeaders-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36

Regex parser process:
Regex parser used 161848 ns
```

`test/basic`包含了两种格式各5条的较为真实的日志。可以看到Parser可以正确地对日志进行解析，而在性能方面和RegexParser差不多（161678ns vs 157940ns、156668ns vs 161848ns；当然每次运行都存在波动，并且该时间还包含了在处理第一条日志前的预处理时间，这部分时间肯定是Parser要长）。

```bash
$ test/stress
Average log length: 164
Parser process:
Parser used 23 ms
Regex parser process:
Regex parser used 247 ms
Average log length: 192
Parser process:
Parser used 28 ms
Regex parser process:
Regex parser used 74 ms
```

`test/stress`包含了两种格式各100'000条随机生成（通过`test/random_log`）的日志。在解析结果完全一致的前提下，Parser是比RegexParser快很多的。

当然，这里生成的日志与真实的日志（很难生成）还是有很大区别的，以下是一条示例：

```plain
x+K]PJ.+o,,BxPo!/h0vfooObz{2;ZIV9~ASn" - 5D= [$U5JzX9vaWX, KjQ]s.}Ww0DAG1[] "*$fQg6:'678]YK4(" 346557263231523068012624 592477117960785835283412873
```

分析：目前来看日志条数越多、日志长度越长，Parser的性能优势就会越大。因为Parser在对第一条日志解析前会花更多时间进行预处理工作，所以在日志条数较少时预处理时间的占比较大（比如在`test/basic`中），而日志长度越长，Parser的优化效果也更明显（见Main Idea）。

## Main Idea

这里介绍Parser的核心思想，作为之前提交的设计文档的补充和一些修正。

### Analysis

虽然是针对特定的tomcat access log日志的解析，但事实上该日志格式还是非常复杂和自由的。它包含十几种格式控制符（format），以及自定义变量的格式控制符（keyformat）。格式控制符之间甚至可能无法区分，比如`%B%D`都是由任意长度的数字组成，如果两者之间不加空格等分隔符，就区分不开。因此用一遍for循环、建立DFA或者编译原理中look ahead等方法是行不通的。

### Strategy

但是像`%B%D`这种格式，我们可以认为它是“糟糕”的。常见的格式基本都是分隔明确，有单一语义的（如`%B %D`）。所以我希望相比于直接使用正则的通用方法，Parser能够在大部分常见格式上有更好的性能，同时在小部分“糟糕”的格式上保持不差的性能以及相同的语义。

具体方法就是利用格式控制符以及格式控制符之间的字面量的特征（长度、左右分隔符、字符集合）进行提前的定位分隔，在无法进行分隔时，再交给正则表达式处理（此时规模往往会更小）。

### Algorithm

具体见`parser.cpp:Process()`。

下面以默认的格式`%h %l %u %t "%r" %s %b`作为讲述的示例。

1. `GenPatterns()`

    首先将给定的格式`format`解析为一个模式数组`std::vector<std::unique_ptr<Pattern>> m_patterns`。

    其中每个格式控制符对应一个正则模式（RegexPattern），格式控制符间的字面量分别对应一个字面量模式（LiteralPattern）。

    比如`%h`用一个正则模式表示，其中包含`%h`的正则表达形式、字符集合、键值描述、长度、左右分隔符等信息；`%h`和`%l`间的空格用一个字面量模式表示，其中包含对应字面量的内容、字符集合、长度、左右分隔符等信息（具体见`pattern_factory.cpp`）。

2. `SplitPatterns()`

    根据相邻模式的特征，对模式数组进行提前分隔，这里从前往后和从后往前分别进行一次。并将分隔的信息`struct SplitSpec`记录到`std::vector<SplitSpec> m_split_specs, m_split_specs_rev`中，并用`std::vector<uint8_t> m_can_split`记录第$i$个模式和第$i+1$个模式能否分隔。

    这里根据三种特征进行分隔。

      + 当前模式的长度固定
      + 下一个模式的左分隔符不在当前模式的字符集合中
      + 下一个模式的字符集合与当前模式的字符集合无交集

    比如正则模式`%h`的正则表达式是`(\S+)`，而下一个字面量模式的左分隔符（即空格）不在其中，因此两者可以直接分隔。又因为空格这个字面量模式的长度为1，又可以与`%l`进行分隔。

    通过正反两次分隔，该默认格式可以实现完全的分隔（即每个相邻的模式都能被分隔开来）。

3. `GenPatternGroups()`

    根据上一步分隔的信息，生成模式组`struct PatternGroup`。每个模式组对应一个完全分隔的正则模式，或者一组连续的模式（包含正则和字面量模式），并记录该组的正则表达形式，编译到对应的`std::regex regex`中，并在`std::vector<RegexPattern*> regex_patterns`中记录该组对应的所有正则模式，用于解析时记录结果。

至此，预处理工作完成。下面对输入的日志逐一解析。

4. `SplitSingleLine()`

    按照刚才的分隔信息，对输入的日志`line`进行分隔，将相邻模式在日志中的分隔点记录在`std::vector<size_t> m_split_line_indices`中。（该过程至多对`line`进行两次遍历）

    比如日志`192.168.1.1 - user2`，可以得到`m_split_line_indices[0] = 10, m_split_line_indices[1] = 11, m_split_line_indices[2] = 12`。

5. `ProcessSingleLine()`

    根据`m_pattern_groups`和`m_split_line_indices`，可以知道每个模式组对应了输入日志中的哪个子区间。比如`%u`对应了区间`[0, 10]`。

    如果当前模式组只有一个正则模式（即已经提前完全分隔），且该正则模式是完全捕获的，那么它对应的区间就是匹配结果。

    否则，再利用`std::regex_match`将该模式组的正则表达式与对应子区间进行匹配，然后得到其中每个正则模式的匹配结果。

### Q&A

1. 为什么要先把 format 切分成 control 数组？直接使用双指针的方法可以实现同样的效果吗？

    A：这里说的是`GenPatterns()`这一步。肯定是在正式进行日志解析前对格式进行提前处理以获取信息的，而不是在每次解析时才进行。对格式的解析就是单个for循环实现的（也可以认为是双指针）。

2. 回溯问题是否可以通过贪婪匹配的形式简化？回溯导致的性能开销是否能优于正则存疑。

    A：这里可能有些误解。我想说的是有些复杂的格式会涉及到回溯（在相邻的模式无法分隔时可能出现），此时我借助正则表达式来完成解析。

    正则表达式作为标准库是高度优化的，我自己去实现这里的回溯，一方面是性能可能反而不好，另一方面是可能引入更多的bugs，不好维护。

3. 当两次遍历得到的匹配结果不一致时，正则一定可以解决这一问题吗？

    A：这里说的是在正着进行一次`SplitPatterns`和反着进行一次`SplitPatternsReverse`后可能存在一些模式之间是无法分隔的，比如`%B%D`，这时我就借助正则表达式来对其进行解析。

    当然，`%B%D`是个“糟糕”的格式，它的解析有多种语义，我使用正则可以保持与原有通用正则方式相同的语义，即都使用贪婪或者非贪婪语义。但是如果我自己实现，解析的语义则可能不同，且不好解释。

### 补充说明

显然，Parser的性能与`pattern_factory.cpp`中对各个格式控制符的特征的记录有很大关系。

目前我采用的还是相对保守的策略。因为很多格式控制符具体可能包含哪些形式在文档中是没有明确记录的。我只对`%r`的字符集合进行了精细的控制，因为`%r`出现在默认格式中。而像`%t`、`%{xxx}o`、`%{xxx}c`等等我都是默认其可以包含任何字符。因此这里还有很多可以根据具体日志进行调优的空间。


## Code Structure

```bash
$ tree
.
├── character_set.cpp
├── character_set.h # 代表一个字符集合：bitset<128>
├── log.cpp
├── log.h # 具体的日志结构，即给定的Log和Content（KV对）
├── Makefile
├── parser.cpp # 核心解析算法
├── parser.h
├── pattern.cpp
├── pattern_factory.cpp # 给定一段格式控制符，制造一个对应的模式
├── pattern_factory.h
├── pattern.h # 模式，又分为字面量模式（格式中固定的字面量）和正则模式（格式中的控制符）
├── README.md
├── test
│   ├── basic.cpp # 基础测试
│   ├── random_log.cpp
│   ├── random_log.h # 生成随机的格式和对应的日志（较简陋）
│   ├── regex_parser.cpp # 完全用正则实现的解析算法（baseline）
│   ├── regex_parser.h
│   └── stress.cpp # 性能测试
├── util.cpp
└── util.h # 一些工具函数
```

## Features

1. 支持[tomcat access log文档](https://tomcat.apache.org/tomcat-8.5-doc/config/valve.html#Extended_Access_Log_Valve)中的所有格式。
2. 可扩展性强，易于修改。只需要更改`pattern_factory`中各个格式控制符的特征即可。
3. 可以使用C++11编译（ilogtail目前也是用的`-std=c++11`的编译选项，因此没有用更新的特性）。

## TODO

1. 添加单元测试。
2. 如果后续有统一的测试集，可以对格式特征以及优化策略进行更细粒度的调整。
