# lau/all.h

切換到其他語言： [English](all_en.md)

`lau/all.h` 標頭檔包含了所有 Lau CPP Library 中的內容。

## 類（以字母順序排列）
- [EmptyContainer](exception_zh.md)：表明此爲空容器
- [Exception](exception_zh.md)：專門處理異常的基類
- [FileDoubleUnrolledLinkedList](file_double_unrolled_linked_list_zh.md)：基於儲存空間、使用塊狀鏈結串列的雙鍵映射表
- [FileUnrolledLinkedList](file_unrolled_linked_list_zh.md)：基於儲存空間、使用塊狀鏈結串列的單鍵映射表
- [InvalidArgument](exception_zh.md)：表明存在非法參數
- [InvalidIterator](exception_zh.md)：表明非法迭代器
- [KeyValuePair](key_value_pair_zh.md)：鍵值對
- [OutOfRange](exception_zh.md)：表明操作越界
- [Pair](pair_zh.md)：含兩個元素的包裹類
- [PriorityQueue](priority_queue_zh.md)：一個以優先級作爲出列依據的佇列（優先佇列）
- [RBTree](RB_tree_zh.md)：紅黑樹類
- [RehashPolicy](rehash_policy_zh.md)：哈希表的重新哈希策略
- [RuntimeError](exception_zh.md)：表明運行時發成錯誤
- [SparseTable](sparse_table_zh.md)：預處理數據以減少可重複貢獻問題的稀疏表
- [TokenScanner](token_scanner_zh.md)：以客製化分隔符掃描並分割爲多個字段
- [Vector](vector_zh.md)：綫性容器（動態數組）

## 函數（以字母順序排列）
- [`PartialMatchTable`](algorithm_zh.md#PartialMatchTable)：計算
  KMP 算法中部分匹配表
- [`StringMatching`](algorithm_zh.md#StringMatching)：以
  KMP 算法匹配字符串
- [`StringMatchingWithPartialMatchTable`](algorithm_zh.md#StringMatchingWithPartialMatchTable)：透過已經處理完畢的部分匹配表作字符串匹配
