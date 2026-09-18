/**
 * @file        utils/traitstest.hpp
 * @brief       Test suite for the type traits library
 * @author      Justin Scott
 * @date        2026-09-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/traits.hpp>

#include <QtTest>

using namespace utils::traits;

///////////////////////////////////////////////////////////////////////////////
/// Class with methods defined to test the memberFuncTraits type trait
///
///////////////////////////////////////////////////////////////////////////////
class TraitsTestClass
    {
public:
    void oopsAllVoid () {}

    std::string lotsOfParams (int           p1,
                              const QPoint& p2,
                              QPoint&       p3,
                              QPoint        p4,
                              bool          p5) { return ""; }


    std::string lotsOfParamsConst (int           p1,
                                   const QPoint& p2,
                                   QPoint&       p3,
                                   QPoint        p4,
                                   bool          p5) const { return ""; }

    int overload (std::string str) { return 0; }

    std::string overload (int num) { return ""; }
    };

///////////////////////////////////////////////////////////////////////////////
/// Test the value and type of null for a pointer like type
///
/// @tparam     Ptr         Pointer like type
///
/// @see        utils::traits::null
///
///////////////////////////////////////////////////////////////////////////////
template<class Ptr>
static void asserNullPointer ()
    {
    QCOMPARE (null<Ptr>, NULL);
    QVERIFY (std::is_null_pointer_v<decltype (null<Ptr>)>);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the type traits library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class TypeTraitsTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test memberFuncTraits with a function with void parameters and return type
    ///
    /// @see    utils::traits::memberFuncTraits
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void memberFuncTraitsVoidTest ()
        {
        using trait = memberFuncTraits<decltype (&TraitsTestClass::oopsAllVoid)>;

        QVERIFY (not trait::is_const);
        QVERIFY (std::is_void_v<trait::ret_t>);
        QVERIFY ((std::is_same_v<TraitsTestClass, trait::class_t>));

        QCOMPARE (std::tuple_size_v<trait::args_t>, 0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test memberFuncTraits with a function with multiple parameters
    ///
    /// @see    utils::traits::memberFuncTraits
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void memberFuncTraitsParamsTest ()
        {
        using trait = memberFuncTraits<decltype (&TraitsTestClass::lotsOfParams)>;

        QVERIFY (not trait::is_const);
        QVERIFY ((std::is_same_v<trait::ret_t, std::string>));
        QVERIFY ((std::is_same_v<TraitsTestClass, trait::class_t>));

        QCOMPARE (std::tuple_size_v<trait::args_t>, 5);

        QVERIFY ((std::is_same_v<std::tuple_element_t<0, trait::args_t>, int>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<1, trait::args_t>, const QPoint&>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<2, trait::args_t>, QPoint&>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<3, trait::args_t>, QPoint>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<4, trait::args_t>, bool>));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test memberFuncTraits with a const function with multiple parameters
    ///
    /// @see    utils::traits::memberFuncTraits
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void memberFuncTraitsParamsConstTest ()
        {
        using trait = memberFuncTraits<decltype (&TraitsTestClass::lotsOfParamsConst)>;

        QVERIFY (trait::is_const);
        QVERIFY ((std::is_same_v<trait::ret_t, std::string>));
        QVERIFY ((std::is_same_v<TraitsTestClass, trait::class_t>));

        QCOMPARE (std::tuple_size_v<trait::args_t>, 5);

        QVERIFY ((std::is_same_v<std::tuple_element_t<0, trait::args_t>, int>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<1, trait::args_t>, const QPoint&>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<2, trait::args_t>, QPoint&>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<3, trait::args_t>, QPoint>));
        QVERIFY ((std::is_same_v<std::tuple_element_t<4, trait::args_t>, bool>));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test memberFuncTraits with an overloaded member function
    ///
    /// @see    utils::traits::memberFuncTraits
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void memberFuncTraitsOverloadTest ()
        {
        auto overload1 = QOverload<int>::of (&TraitsTestClass::overload);
        auto overload2 = QOverload<std::string>::of (&TraitsTestClass::overload);

        using trait1 = memberFuncTraits<decltype (overload1)>;
        using trait2 = memberFuncTraits<decltype (overload2)>;

        // Overload 1 tests

        QVERIFY (not trait1::is_const);
        QVERIFY ((std::is_same_v<trait1::ret_t, std::string>));
        QVERIFY ((std::is_same_v<TraitsTestClass, trait1::class_t>));

        QCOMPARE (std::tuple_size_v<trait1::args_t>, 1);
        QVERIFY ((std::is_same_v<std::tuple_element_t<0, trait1::args_t>, int>));

        // Overload 2 tests

        QVERIFY (not trait2::is_const);
        QVERIFY ((std::is_same_v<trait2::ret_t, int>));
        QVERIFY ((std::is_same_v<TraitsTestClass, trait2::class_t>));

        QCOMPARE (std::tuple_size_v<trait2::args_t>, 1);
        QVERIFY ((std::is_same_v<std::tuple_element_t<0, trait2::args_t>, std::string>));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the type eneveloper
    ///
    /// @see    utils::traits::envelope
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void envelopeTest ()
        {
        QVERIFY ((std::is_same_v<envelope<int>::type, int>));
        QVERIFY ((std::is_same_v<envelope<int&>::type, int&>));
        QVERIFY ((std::is_same_v<envelope<int&&>::type, int&&>));
        QVERIFY ((std::is_same_v<envelope<const int&>::type, const int&>));
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test the always false value
    ///
    /// @see    utils::traits::always_false_v
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void alwaysFalseTest ()
        {
        QVERIFY (not always_false_v<int>);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the templated NULl constant
    ///
    /// @see    utils::traits::null
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void nullTest ()
        {
        static const std::optional<int> NULLOPT = std::nullopt;

        asserNullPointer<int*> ();
        asserNullPointer<std::unique_ptr<int>> ();
        asserNullPointer<std::weak_ptr<int>> ();
        asserNullPointer<std::shared_ptr<int>> ();

        QCOMPARE (null<std::optional<int>>,     NULLOPT);
        QVERIFY ((std::is_same_v<std::nullopt_t, decltype (null<std::optional<int>>)>));
        }
    };

QTEST_GUILESS_MAIN (TypeTraitsTest)

#include "traitstest.moc"
