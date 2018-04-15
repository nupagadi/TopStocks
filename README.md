The project contains two executables: UnitTests and Display. The former launches all the unit tests, the latter - simple display unit, which shows top rankers using implemented TopStocks class. 

Implementation

All the stocks are stored in a hash-table including stock id, base and last percent change. Because there is no need in keeping all the stocks ordered by percent change, only the topmost 16 at each side are ordered. Keeping more than 10 elements ordered allows not to search 10000 elements for 10th biggest or smallest every time when the top ranker leaves the chart. The value 16 is empirical.

To keep the topmost up to date two types of thresholds are used. First one is value of 10th element, second one is value of last ordered element (usually 14th, 15th or 16th). The first shows if the corresponding chart was altered and the notification should be raised. The second indicates whether the element should be added or removed from the topmost 16 (but may be with no notification).

Sometimes when there are many elements with the same percent value in the top (e.g. at the start when all the values are 0), notifications can be raised even if the top haven't changed. It is rare and I cannot imagine the case when it could be harmful. In the real world situation I'd discuss such a possibility.

Complexity

The algorithm was developed under the assumption that the top rankers seldom massively leaves the chart. If that's the case the complexity of the algorithm is const (the cost of adding or removing from the red-black tree with the size limited to 16). Otherwise the topmost is reset and the complexity of this operation is O(N).

Used Tools: MS Visual C++ Compiler 14.0 x86, Qt Creator 4.1, Windows 7 x32.
