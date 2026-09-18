/**
 * @file        utils/algorithmtest.hpp
 * @brief       Test suite for the algorithm library
 * @author      Justin Scott
 * @date        2026-08-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <testutils/ext/fakeit.hpp>

#include <utils/algorithm.hpp>

#include <QtTest>

using namespace utils::algorithm;
using namespace std::string_literals;

///////////////////////////////////////////////////////////////////////////////
/// Class for testing bindMemFn
///
/// @see    utils::algorithm::bindMemFn()
/// @see    AlgorithmTest::bindMemFn()
///
///////////////////////////////////////////////////////////////////////////////
class MemberFuncTest
    {
public:
    virtual void setStr (std::string str) = 0;

    virtual void setInt (int myInt) = 0;

    virtual void setPnt (QPoint pt) = 0;

    virtual std::string getStr () const = 0;

    virtual int getInt () const = 0;

    virtual QPoint getPnt () const = 0;


    };

/// Struct for testing the safeGet() function
struct safeGetTestRec
    {
    int         num;    ///< Number
    std::string str;    ///< String
    QPoint      pt;     ///< Point
    };


///////////////////////////////////////////////////////////////////////////////
/// Helper function for assertSafeGetNullPointer() which tests just one pointer
/// type
///
/// @tparam     Ptr         Pointer type
/// @tparam     MemType     Member variable type
/// @tparam     Elem        Pointer element type
///
/// @param[in]  memVar      Member variable
/// @param[in]  defaultVal  Default value to pass to safeGet()
///
///////////////////////////////////////////////////////////////////////////////
template<class Ptr, class MemType, class Elem = typename std::pointer_traits<Ptr>::element_type>
static void assertSafeGetNullPointerHelper (MemType Elem::*                 memVar,
                                            const identityType<MemType>&    defaultVal)
    {
    Ptr ptr = utils::traits::null<Ptr>;

    MemType actual = safeGet (ptr, memVar, defaultVal);

    QCOMPARE (actual, defaultVal);
    }

///////////////////////////////////////////////////////////////////////////////
/// Tests safeGet() when the pointer is NULL for multiple pointer like types
///
/// @tparam     T           Type to test pointer like types for
/// @tparam     MemType     Member variable type
///
/// @param[in]  memVar      Member variable
/// @param[in]  defaultVal  Default value to pass to safeGet()
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class MemType>
static void assertSafeGetNullPointer (MemType T::*                  memVar,
                                      const identityType<MemType>&  defaultVal)
    {
    assertSafeGetNullPointerHelper<T*>                  (memVar, defaultVal);
    assertSafeGetNullPointerHelper<std::unique_ptr<T>>  (memVar, defaultVal);
    assertSafeGetNullPointerHelper<std::shared_ptr<T>>  (memVar, defaultVal);
    assertSafeGetNullPointerHelper<std::optional<T>>    (memVar, defaultVal);
    }

///////////////////////////////////////////////////////////////////////////////
/// Make a pointer-like type
///
/// @tparam     Ptr     Pointer like type
///
/// @return     Newly created pointer type
///
///////////////////////////////////////////////////////////////////////////////
template<class Ptr>
static Ptr makePointer ()
    {
    using Elem = typename std::pointer_traits<Ptr>::element_type;

    if constexpr (std::is_same_v<Ptr, std::optional<Elem>>)
        {
        return Elem{};
        }
    else
        {
        return Ptr{ new Elem{} };
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Helper function for assertSafeGetMemberHelper() which tests just one pointer
/// type
///
/// @tparam     Ptr         Pointer type
/// @tparam     MemType     Member variable type
/// @tparam     Elem        Pointer element type
///
/// @param[in]  memVar      Member variable
/// @param[in]  defaultVal  Default value to pass to safeGet()
///
///////////////////////////////////////////////////////////////////////////////
template<class Ptr, class MemType, class Elem = typename std::pointer_traits<Ptr>::element_type>
static void assertSafeGetMemberHelper (MemType Elem::* memVar, const identityType<MemType>& expected)
    {
    Ptr ptr = makePointer<Ptr> ();

    (*ptr).*memVar = expected;

    QCOMPARE (safeGet (ptr, memVar), expected);

    if constexpr (std::is_pointer_v<Ptr>)
        {
        delete ptr;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Tests safeGet() when the pointer is not NULL for multiple pointer like types
///
/// @tparam     T           Type to test pointer like types for
/// @tparam     MemType     Member variable type
///
/// @param[in]  memVar      Member variable
/// @param[in]  expected    Expected return value
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class MemType>
static void assertSafeGetMember (MemType T::* memVar, const identityType<MemType>& expected)
    {
    assertSafeGetMemberHelper<T*>                   (memVar, expected);
    assertSafeGetMemberHelper<std::unique_ptr<T>>   (memVar, expected);
    assertSafeGetMemberHelper<std::shared_ptr<T>>   (memVar, expected);
    assertSafeGetMemberHelper<std::optional<T>>     (memVar, expected);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test case implementation for a single member variable
///
/// @tparam     T           Type to test pointer like types for
/// @tparam     MemType     Member variable type
///
/// @param[in]  memVar      Member variable
/// @param[in]  expected    Expected return value
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class MemType>
static void safeGetTestImpl (MemType T::* memVar, const identityType<MemType>& expected)
    {
    assertSafeGetNullPointer    (memVar, expected);
    assertSafeGetMember         (memVar, expected);
    }


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the algorithm library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AlgorithmTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the utils::algorithm::makeBitset() function
    ///
    /// @see    utils::algorithm::makeBitset()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void makeBitsetTest ()
        {
        static constexpr int NUM_BITS = 10;

        std::bitset<NUM_BITS> actual;

        for (int ii = 0; ii < NUM_BITS; ++ii)
            {
            for (int jj = 0; jj < NUM_BITS; ++jj)
                {
                for (int kk = 0; kk < NUM_BITS; ++kk)
                    {
                    makeBitset (actual, ii, jj, kk);

                    for (int bit = 0; bit < NUM_BITS; ++bit)
                        {
                        QCOMPARE (actual[bit],
                                  bit == ii ||
                                  bit == jj ||
                                  bit == kk);
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the utils::algorithm::bitsetToSet() function
    ///
    /// @see    utils::algorithm::bitsetToSet()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void bitsetToSetTest ()
        {
        static constexpr int NUM_BITS = 10;

        std::bitset<NUM_BITS> bits;

        for (int ii = 0; ii < NUM_BITS; ++ii)
            {
            for (int jj = 0; jj < NUM_BITS; ++jj)
                {
                for (int kk = 0; kk < NUM_BITS; ++kk)
                    {
                    makeBitset (bits, ii, jj, kk);

                    std::set<size_t> set = bitsetToSet (bits);

                    for (int bit = 0; bit < NUM_BITS; ++bit)
                        {
                        QCOMPARE (bits[bit], set.end () != set.find (bit));
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the contains function on a C array
    ///
    /// @see     utils::algorithm::contains()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void containsTest ()
        {
        static const int TEST_ARRAY[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

        QVERIFY (contains (TEST_ARRAY, 0));
        QVERIFY (contains (TEST_ARRAY, 1));
        QVERIFY (contains (TEST_ARRAY, 5));

        QVERIFY (not contains (TEST_ARRAY, 12));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the findArrayIdx function on a C array
    ///
    /// @see     utils::algorithm::findArrayIdx()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findArrayIdxTest ()
        {
        static const int TEST_ARRAY[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

        QCOMPARE (findArrayIdx (TEST_ARRAY, 0), 0);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 1), 1);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 1), 1);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 55), std::size (TEST_ARRAY) - 1);

        QCOMPARE (findArrayIdx (TEST_ARRAY, 12), std::size (TEST_ARRAY));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Simple test of the EnumRange and it's associated iterator
    ///
    /// @see    utils::algorithm::EnumRange
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void enumRangeTest ()
        {
        enum test
            {
            TEST_1,
            TEST_2,
            TEST_3,
            TEST_4,
            TEST_5
            };

        EnumRange range{ TEST_1, TEST_5 };

        // Test of begin and end
        QCOMPARE (*range.begin (), TEST_1);
        QCOMPARE (*range.end (), TEST_5);

        auto it = range.begin ();

        // Test comparison operators
        QCOMPARE (it, range.begin ());
        QCOMPARE_NE (it, range.end ());

        // Test iterator advancement
        ++it;
        QCOMPARE (*it, TEST_2);

        it++;
        QCOMPARE (*it, TEST_3);

        ++it;
        QCOMPARE (*it, TEST_4);

        it++;
        QCOMPARE (*it, TEST_5);

        QCOMPARE (it, range.end ());
        QCOMPARE_NE (it, range.begin ());


        it--;
        QCOMPARE (*it, TEST_4);

        --it;
        QCOMPARE (*it, TEST_3);

        --it;
        QCOMPARE (*it, TEST_2);

        --it;
        QCOMPARE (*it, TEST_1);

        QCOMPARE (it, range.begin ());
        QCOMPARE_NE (it, range.end ());

        // Test advancement iterator order
        QCOMPARE (it++, range.begin ());

        auto it2 = it = range.begin ();
        ++it2;

        QCOMPARE (++it, it2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test binding member functions to an object
    ///
    /// @see    utils::algorithm::bindMemFn()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void bindMemFnTest ()
        {
        static std::string  TEST_SET_STRING = "Test String";
        static int          TEST_SET_INT    = 42;
        static QPoint       TEST_SET_POINT  = { 67, 43 };

        static std::string  TEST_GET_STRING = "Other String";
        static int          TEST_GET_INT    = 24;
        static QPoint       TEST_GET_POINT  = { 47, 66 };

        // Setup

        fakeit::Mock<MemberFuncTest>    mock;
        MemberFuncTest&                 obj     = mock.get ();
        const MemberFuncTest&           cobj    = obj;

        fakeit::When (Method (mock, setStr)).AlwaysReturn ();
        fakeit::When (Method (mock, setInt)).AlwaysReturn ();
        fakeit::When (Method (mock, setPnt)).AlwaysReturn ();

        fakeit::When (Method (mock, getStr)).AlwaysReturn (TEST_GET_STRING);
        fakeit::When (Method (mock, getInt)).AlwaysReturn (TEST_GET_INT);
        fakeit::When (Method (mock, getPnt)).AlwaysReturn (TEST_GET_POINT);

        // Bind calls

        auto setStr = bindMemFn (&MemberFuncTest::setStr, obj);
        auto setInt = bindMemFn (&MemberFuncTest::setInt, obj);
        auto setPnt = bindMemFn (&MemberFuncTest::setPnt, obj);

        auto getStr = bindMemFn (&MemberFuncTest::getStr, cobj);
        auto getInt = bindMemFn (&MemberFuncTest::getInt, cobj);
        auto getPnt = bindMemFn (&MemberFuncTest::getPnt, cobj);

        // Test setters

        setStr (TEST_SET_STRING);
        setInt (TEST_SET_INT);
        setPnt (TEST_SET_POINT);

        fakeit::Verify (Method (mock, setStr).Using (TEST_SET_STRING));
        fakeit::Verify (Method (mock, setInt).Using (TEST_SET_INT));
        fakeit::Verify (Method (mock, setPnt).Using (TEST_SET_POINT));

        // Test getters

        QCOMPARE (getStr (), TEST_GET_STRING);
        QCOMPARE (getInt (), TEST_GET_INT);
        QCOMPARE (getPnt (), TEST_GET_POINT);

        fakeit::Verify (Method (mock, getStr)).Once ();
        fakeit::Verify (Method (mock, getInt)).Once ();
        fakeit::Verify (Method (mock, getPnt)).Once ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for safeGetIntTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetIntTest_data ()
        {
        QTest::addColumn<int> ("num");

        for (int ii = -1234; ii < 2567; ++ii)
            {
            QTest::addRow ("%d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test safeGet with an int member variable
    ///
    /// @see    utils::algorithm::safeGet()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetIntTest ()
        {
        QFETCH (int, num);

        safeGetTestImpl<safeGetTestRec> (&safeGetTestRec::num, num);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for safeGetStringTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetStringTest_data ()
        {
        QTest::addColumn<std::string> ("str");

        QTest::newRow ("Test")  << "Test"s;
        QTest::newRow ("Empty") << ""s;
        QTest::newRow ("Long")  << "lorem ipsum dolor sit amet consectetur adipiscing elit est quis irure consequatur duis sint illum excepteur in ullamco vero corrupti provident in praesentium et consectetur aliqua culpa quis ut officia qui deserunt consequat aliqua cupiditate eos voluptatum ea dolor nulla quibusdam voluptas similique et facere cum pariatur maxime est est"s;
        QTest::newRow ("UTF-8") << "测试文本"s;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test safeGet with a string member variable
    ///
    /// @see    utils::algorithm::safeGet()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetStringTest ()
        {
        QFETCH (std::string, str);

        safeGetTestImpl<safeGetTestRec> (&safeGetTestRec::str, str);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for safeGetPointTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetPointTest_data ()
        {
        QTest::addColumn<QPoint> ("pt");

        for (int ii = -100; ii < 100; ++ii)
            {
            for (int jj = -100; jj < 100; ++jj)
                {
                QTest::addRow ("(%d,%d)", ii, jj) << QPoint{ ii, jj };
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test safeGet with a point member variable
    ///
    /// @see    utils::algorithm::safeGet()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void safeGetPointTest ()
        {
        QFETCH (QPoint, pt);

        safeGetTestImpl<safeGetTestRec> (&safeGetTestRec::pt, pt);
        }
    };


QTEST_GUILESS_MAIN (AlgorithmTest);

#include "algorithmtest.moc"
