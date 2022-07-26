# Wiki

切換到其他語言： [English](wiki_main_en.md)

## 標頭檔（以字母順序排列）
- [all.h](wiki/all_zh.md)：包含所有 Lau CPP Library 中的內容
- [exception.h](wiki/exception_zh.md)：包含所有 Lau CPP Library 中的異常類
- [file_data_structure.h](wiki/file_data_structure_zh.md)：包含所有 Lau CPP Library
  中基於儲存空間的數據結構
- file_double_unrolled_linked_list.h：包含類
  [`FileDoubleUnrolledLinkedList`](wiki/file_double_unrolled_linked_list_zh.md)
- file_unrolled_linked_list.h：包含類
  [`FileUnrolledLinkedList`](wiki/file_unrolled_linked_list_zh.md)
- priority_queue.h：包含類 [`PriorityQueue`](wiki/priority_queue_zh.md)
- RB_tree.h：包含類 [`RBTree`](wiki/RB_tree_zh.md)
- rehash_policy.h：包含類 [`RehashPolicy`](wiki/rehash_policy_zh.md)
- sparse_table.h：包含類 [`SparseTable`](wiki/sparse_table_zh.md)
- token_scanner.h：包含類 [`TokenScanner`](wiki/token_scanner_zh.md)
- [type_trait.h](wiki/type_trait_en.md)：包含所有 Lau CPP Library 中的類型特性
- [utility.h](wiki/utility_zh.md)：包含用以提昇效率的工具
- vector.h：包含類 [`Vector`](wiki/vector_zh.md)

## 類（以字母順序排列）
- [EmptyContainer](wiki/exception_zh.md)：表明此爲空容器
- [Exception](wiki/exception_zh.md)：專門處理異常的基類
- [FileDoubleUnrolledLinkedList](wiki/file_double_unrolled_linked_list_zh.md)：基於儲存空間、使用塊狀鏈結串列的雙鍵映射表
- [FileUnrolledLinkedList](wiki/file_unrolled_linked_list_zh.md)：基於儲存空間、使用塊狀鏈結串列的單鍵映射表
- [InvalidArgument](wiki/exception_zh.md)：表明存在非法參數
- [InvalidIterator](wiki/exception_zh.md)：表明非法迭代器
- [KeyValuePair](wiki/key_value_pair_zh.md)：鍵值對
- [OutOfRange](wiki/exception_zh.md)：表明操作越界
- [Pair](wiki/pair_zh.md)：含兩個元素的包裹類
- [PriorityQueue](wiki/priority_queue_zh.md)：一個以優先級作爲出列依據的佇列（優先佇列）
- [RBTree](wiki/RB_tree_zh.md)：紅黑樹類
- [RehashPolicy](wiki/rehash_policy_zh.md)：哈希表的重新哈希策略
- [RuntimeError](wiki/exception_zh.md)：表明運行時發成錯誤
- [SparseTable](wiki/sparse_table_zh.md)：預處理數據以減少可重複貢獻問題的稀疏表
- [TokenScanner](wiki/token_scanner_zh.md)：以客製化分隔符掃描並分割爲多個字段
- [Vector](wiki/vector_zh.md)：綫性容器（動態數組）

## 函數（以字母順序排列）
- [`PartialMatchTable`](wiki/algorithm_zh.md#PartialMatchTable)：計算
  KMP 算法中部分匹配表（位於 `algorithm.h` 標頭檔中）
- [`StringMatching`](wiki/algorithm_zh.md#StringMatching)：以
  KMP 算法匹配字符串（位於 `algorithm.h` 標頭檔中）
- [`StringMatchingWithPartialMatchTable`](wiki/algorithm_zh.md#StringMatchingWithPartialMatchTable)：透過已經處理完畢的部分匹配表作字符串匹配（位於
  `algorithm.h` 標頭檔中）
