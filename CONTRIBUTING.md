# Contributing to Hyrax Rail

Hyrax rail is still in it's early phases with an ever-evolving design. Code contribution is not yet allowed until it stabilizes. Much of this document therefore is a draft for when I open for contributions. However, opening issue tickets is encouraged.

## Coding Standard

This section outlines the coding standard to follow while making contributions to this project.

### Whitespace Settings and File Encodings

Contributions should be made with:
 * Spaces, not tabs
 * Four space tab width
 * Trailing whitespace trimmed (see trailing whitespace)
 * UTF-8 encoded (except `*.rc` files)
 * UTF-16 encoded `*.rc` files
 
### Automated Whitespace Removal

As the coding standard requires trimming of trailing whitespace, the following plugins are reccomended to ensure commits are properly trimmed:

| Text Editor/IDE      | Plugin       | Author          |
|----------------------|--------------|-----------------|
| Visual Studio(2017)  | [Trailing Whitespace Visualizer](https://marketplace.visualstudio.com/items?itemName=MadsKristensen.TrailingWhitespaceVisualizer) | [Mads Kristensen](https://github.com/madskristensen) |
| Visual Studio(2022/2026)  | [Trailing Whitespace Visualizer](https://marketplace.visualstudio.com/items?itemName=MadsKristensen.TrailingWhitespace64) | [Mads Kristensen](https://github.com/madskristensen) |
| VS Code              | [Trailing Spaces](https://marketplace.visualstudio.com/items?itemName=shardulm94.trailing-spaces) | [Shardul Mahadik](https://github.com/shardulm94) |

### Coding Style (C++)

All c++ code should use the [Whitesmith](https://en.wikipedia.org/wiki/Indentation_style#Whitesmiths) brace style.

```
if (condition)
    {
    doSomething ();
    }
```

A space should be place before open parentheses. Within the parentheses, a space should be placed after the comma. 

```
funcCall (var1, var2, var3);
```

When using brace intialization, no space shall be placed before the opening brace. A space shall be placed before the first element, after last, and after each comma.

```
MyClass{ var1, var2, var3 };
```

When using template parameters, no space should be place before the angle bracket, or before the first element, or after the last element. When multilple template parameters are specificed, a space should be placed after the comma.

```
template<class Type1, class Type2>
void foo ();

...

foo<MyType1, MyType2> ();
```

When using the index operator, no space should be placed before the square bracket.

```
int myArr[4]

myArr[1] = 0;
```

When using pointers and references, the `*` and `&` should be placed on the left, next to the type name;

```
int     myInt = 4;
int*    myPtr = &myInt;
int&    myRef = *myPtr;
```

Namepaces should not be indented. A comment should be added to the closing brace to help identify namespace scopes

```
namespace myNamespace
{
int myGlobal;

void myFunc ();
} // namespace myNamespace
```

Access specifiers in class should not be indented.

```
class MyClass
    {
public:
    MyClass ();
private:
    int myVar;
    };
```

Preprocessor blocks should not be indented. A comment should be added to the `#endif` to help identify preprocessor blocks.

```
#if CONDITION_1

#if CONDITION_2
...
#else // !CONDITION_2
... 
#endif // CONDITION_2

#endif // CONDITION_1
```

When naming types and function, camel case should be used. If the type is a class, The first letter should be capatilzed:

```
struct myStruct
    {
    int myField;
    };
    
class MyClass 
    {
public:
    void myMemFunc ();
    };
    
    
void myFunc ();
```

When naming constants, upper case snake case should be used.

```
static const int MY_CONSTANT = 4;
```

When naming private member variables in a class, they should be prefixed with `m_`.

```
class MyClass
    {
    ...
private:
    int     m_myInt;
    double  m_myDouble;
    };
```

When multiple variables are declared together, they should be aligned. When possible, this should be on 4 space boundaries.

```
int     myInt       = 4;
double  myDouble    = 5.0;
MyClass myObj;
```

When initializing objects, brace intilization should be preferred:

```
MyClass myObj{ myVar1, myVar2 };
```

Every file should have a comment header with the following template:

```
/**
 * @file
 * @brief
 * @author
 * @date
 *
 * @copyright
 */

```

Every public class, function and member function should have a doxygen compliant comment header in the header file. This should document all template parameters, function parameters, and returns values.

```
///////////////////////////////////////////////////////////////////////////////
/// This is a class
///
///////////////////////////////////////////////////////////////////////////////
class MyClass
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// This is a member function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void myMemberFunc ();
    };
    
///////////////////////////////////////////////////////////////////////////////
/// This is a template class
///
/// @tparam     T       This is a template parameter
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
class MyTemplateClass
    {};
    
///////////////////////////////////////////////////////////////////////////////
/// This is a function
///
/// @param[in]  myParam     This is a parameter
///
//  @return     This is an integer return value
///
///////////////////////////////////////////////////////////////////////////////
int myFunc (double myParam);

```

Where possible, the coding standard should be captured in .editorconfig. If you believe it is not, please raise an issue.

Where possible, keep line lengths below 100 columns to aid in diffs.

### Best Practices

A few best practices to follow:
 * Avoid mutable global state. The use of meta classes is allowed, as it initialized via global constructors, and then never modified again.
 * The use of goto, setjmp, and longjmp are strictly prohibited.
 * Code must be exception safe, use RAII.
 * Per above, malloc, calloc and ralloc are discouraged except in very specific circumstances, where is carefully managed by an RAII class.

Before starting on any module, please read the README in the module folder. This will outline the design phillosophy of the module, it's background, and some rules to follow while extending it.
