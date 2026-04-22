/**
 * @file        controller/manager.hpp
 * @brief       Provides container to manage controllers, with signals for
 *              when controllers are added, deleted or changed
 * @author      Justin Scott
 * @date        2026-04-20
 *
 * @copyright   Copyright (c) 2026 Justin ScottS
 */

#pragma once

#include <control/controllers/base.hpp>

#include <QObject>

namespace control
{

///////////////////////////////////////////////////////////////////////////////
/// Containter to manager ownership of controllers with signals for when
/// controllers are added, deleted or changed
///
///////////////////////////////////////////////////////////////////////////////
class ControllerManager : public QObject
    {
    Q_OBJECT

    using controllerList = std::vector<std::unique_ptr<ControllerBase>>;

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Controller list iterator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    class Iterator
        {
        friend ControllerManager;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = ControllerBase;
        using difference_type   = typename controllerList::difference_type;
        using pointer           = value_type*;
        using reference         = value_type&;

        // Copyable
        Iterator (const Iterator&) = default;
        Iterator& operator= (const Iterator&) = default;

        // Movable
        Iterator (Iterator&&) = default;
        Iterator& operator= (Iterator&&) = default;

        ///////////////////////////////////////////////////////////////////////////////
        /// Pointer arrow operator
        ///
        /// @return     Pointer to controller
        ///
        ///////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] pointer operator-> () const noexcept { return m_it->get (); }

        ///////////////////////////////////////////////////////////////////////////////
        /// Pointer de-reference operator
        ///
        /// @return     Reference to controller
        ///
        ///////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] reference operator* () const noexcept { return **m_it; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Iterator increment operator
        ///
        /// @return     Reference to this
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator& operator++ () noexcept { m_it++; return *this; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Iterator increment after operator
        ///
        /// @return     Un-incremented iterator
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator operator++ (int) noexcept { auto tmp = *this; m_it++; return tmp; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Iterator decrement operator
        ///
        /// @return     Reference to this
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator& operator-- () noexcept { m_it--; return *this; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Iterator decrement after operator
        ///
        /// @return     Un-decremented iterator
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator operator-- (int) noexcept { auto tmp = *this; m_it--; return tmp; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Add an offset to this iterator
        ///
        /// @param[in]  off     Offset to add to this iterator
        ///
        /// @return     Reference to this
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator& operator+= (const difference_type off) noexcept { m_it += off; return *this; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Subtract an offset from this iterator
        ///
        /// @param[in]  off     Offset to subtract from this iterator
        ///
        /// @return     Reference to this
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator& operator-= (const difference_type off) noexcept { m_it -= off; return *this; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Adds an offset to this iteraor, creating a new iterator
        ///
        /// @param[in]  off     Offset to add to this iterator
        ///
        /// @return     Iterator with offset added
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator operator+ (const difference_type off) const noexcept { return (m_it + off); }

        ///////////////////////////////////////////////////////////////////////////////
        /// Subtract an offset from this iteraor, creating a new iterator
        ///
        /// @param[in]  off     Offset to subtract this iterator
        ///
        /// @return     Iterator with offset subtracted
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator operator- (const difference_type off) const noexcept { return (m_it - off); }

        ///////////////////////////////////////////////////////////////////////////////
        /// Index operator
        ///
        /// @param[in]  off     Inex value
        ///
        /// @return     Reference to controller at index off
        ///
        ///////////////////////////////////////////////////////////////////////////////
        reference operator[] (const difference_type off) const noexcept { return *m_it[off]; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Compare to iterators
        ///
        /// @param[in]  other       Iterator to compare to
        ///
        /// @return     True if iterators point to the same controller
        ///
        ///////////////////////////////////////////////////////////////////////////////
        bool operator== (const Iterator& other) const { return m_it == other.m_it; }

        ///////////////////////////////////////////////////////////////////////////////
        /// Compare to iterators
        ///
        /// @param[in]  other       Iterator to compare to
        ///
        /// @return     True if iterators point to different controllers
        ///
        ///////////////////////////////////////////////////////////////////////////////
        bool operator!= (const Iterator& other) const { return !(*this == other); }
    private:
        using iterImpl = controllerList::iterator;

        iterImpl m_it;  ///< Iterator into list implementation

        ///////////////////////////////////////////////////////////////////////////////
        /// Create a manager iterator from the list iterator
        ///
        /// @param[in]  it      List iterator
        ///
        ///////////////////////////////////////////////////////////////////////////////
        Iterator (iterImpl it) :
            m_it (it)
            {}
        };

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Object owning the controllers
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ControllerManager (QObject* parent) :
        QObject (parent)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Index operator
    ///
    /// @param[in]  idx     Index to get controller for
    ///
    /// @return     Reference to controller at index at idx
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerBase& operator[] (size_t idx) { return *m_controllers[idx].get (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Index operator, const version
    ///
    /// @param[in]  idx     Index to get controller for
    ///
    /// @return     Reference to controller at index at idx
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const ControllerBase& operator[] (size_t idx) const { return *m_controllers[idx].get (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the start iterator
    ///
    /// @return     Start iterator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Iterator begin () { return m_controllers.begin (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the end iterator
    ///
    /// @return     End iterator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Iterator end () { return m_controllers.end (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a controller to the manager
    ///
    /// @param[in]  info        Controller info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void append (const createControllerInfo& info);

    ///////////////////////////////////////////////////////////////////////////////
    /// Remove a controller from the list
    ///
    /// @param[in]  controller      Controller to remove
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void remove (const ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Remove a controller from the list by iterator
    ///
    /// @param[in]  it      Iterator to remove controller for
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void erase (Iterator it);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the number of controllers this is managing
    ///
    /// @return     Number of controllers
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t size () const { return m_controllers.size (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the index of a controller
    ///
    /// @param[in]  controller      Controller to get index of
    ///
    /// @return     Index of controller
    ///             -1 if controller isn't managed by this container
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ptrdiff_t indexOf (const ControllerBase& controller) const;

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals a controller has been added
    ///
    /// @param[in]  controller      Controller that has been added
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void controllerAdded (ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Signals a controller has been deleted
    ///
    /// @param[in]  controller      Controller that has been deleted
    ///
    /// @remarks    If you are connecting this to a slot on a different thread you
    ///             MUST use Qt::BlockingQueuedConnection, as the controller object
    ///             is deleted after emiting this signal. If you are connecting this
    ///             to a slot on the same thread, you must use Qt::DirectConnection,
    ///             or Qt::AutoConnection (default behaviour)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void controllerDeleted (const ControllerBase& controller);

private:

    controllerList m_controllers;   ///< List of controllers we are managing
    };

} // namespace control