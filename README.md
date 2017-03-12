# Btree #

I made this project to practice C++11 and TDD, it is not intended to be used in production environment.
A Btree is a self-balancing tree data structure with *degree* number of keys per node.
It enables read and write in *O(log n)* steps.


"Unlike self-balancing binary search trees, the B-tree is optimized for systems that read and write large blocks of data.
B-trees are a good example of a data structure for external memory. It is commonly used in databases and filesystems."
[Wikipedia article](https://en.wikipedia.org/wiki/B-tree)


#### Features ####

* Add, read, modify data in the tree.
* Provides methods for pre-, in-, postorder walks.
* Dump the data from the tree.

#### Build ####

Compile, link:

`make`

After this step you can run the tests with `./test`

---

Compile, link and run with valgrind:

`make check`

---

Compile with coverage, link, run tests and generate coverage
It is important to clean the project before running this target because the compiler need to be run with `--coverage`:

`make profile`

After finish, the html report can be accessed by coverage/index.html


#### Development diary ####

TDD-ing a datastructure like this is a tough nut because its balancing feature cannot be checked through its interface.
To overcome this problem I needed to create an algorithm which can be used to check wheter the tree is balanced.
This algorithm is implemented by the `Measurable` class, which with the `measure` method can measure the deepest and
shallowest branch of the tree. For balanced trees this two values should be equal. To prove that this algorithm works,
I have created the `MeasurableTree` class on which I can freely manipulate the branches. After finishing the algorithm,
I was able to inherit from the `Measurable` abstract class by the `Btree` class to create the `MeasurableBtree`.
The advantage of this solution is that the public Btree implementation will not be infected by the low level
measurement features which are needed by the unit tests.


After providing a method to check the balancedness of an arbitrary tree, I was able to start the actual implementation
of the Btree. My first goal was the 2-3 Tree, which is a special case of the Btree, in which the *degree* is set to 2.
To make the problem easier I decided that for the first few test cases I will provide the keys in an incrementing way.
I decided that for the time being I will not deal with the different type of keys problem and fixed the type of the
keys to integers.
After finishing the test cases for the incrementing keys, I added cases for adding decrementing keys.
Finishing the decrementing keys cases I added mixed cases which adds keys in an incrementing-decrementing manner.
Just to make sure my method works I added a random test case which adds random keys.
During the development of the 2-3 Tree case it became clear that managing the keys and children is a problem on its own.
So, I moved the whole key and children management logic to the `Keys` class which is a friend of the `Btree` and
is not meant for public use. Because the algorithms mostly work with a key and its left and right children
I developed a way of working with branches where a branch consists of a key and its two children.


Finishing the 2-3 tree case I made the *degree* of the tree arbitrarily big, which actually meant the further
generalization of some of my algorithms. I was able to seperate this problem to two subproblems: first working with
arbitrarily big even number of *degrees* (because the 2-3 tree had an even number of keys as well) and then
solving the problem of odd number of *degrees*.

After this, finishing the tree was a piece of cake, I added the missing details like:

* copy constructor, and the rest of the big four
* storing arbitrary type of keys
* storing arbitrary type of values
* walk methods
