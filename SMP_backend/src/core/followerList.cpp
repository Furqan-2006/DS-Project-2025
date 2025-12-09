#include "core/followerList.hpp"

// Helper function for string comparison
bool stringEquals(const char *s1, const char *s2)
{
    if (!s1 || !s2)
        return false;

    while (*s1 && *s2)
    {
        if (*s1 != *s2)
            return false;
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

FollowerList::FollowerList() {}

FollowerList::~FollowerList()
{
    // Clean up all dynamically allocated linked lists
    for (auto it = followersMap.begin(); it != followersMap.end(); ++it)
    {
        if ((*it).value)
        {
            delete (*it).value;
        }
    }
}

void FollowerList::addFollower(ull userID, ull followerID)
{
    LinkedList<ull> **listPtr = followersMap.get(userID);

    if (!listPtr)
    {
        // Create new linked list for this user
        LinkedList<ull> *newList = new LinkedList<ull>();
        followersMap.insert(userID, newList);
        newList->append(followerID);
    }
    else
    {
        LinkedList<ull> *list = *listPtr;

        // Check if follower already exists using linked-list traversal
        Node<ull> *current = list->head();
        while (current)
        {
            if (current->data == followerID)
            {
                return; // Already exists
            }
            current = current->next;
        }

        // Add new follower
        list->append(followerID);
    }
}

void FollowerList::removeFollower(ull userID, ull followerID)
{
    LinkedList<ull> **listPtr = followersMap.get(userID);

    if (!listPtr || !*listPtr)
        return;

    // Remove from linked list
    (*listPtr)->remove(followerID);
}

LinkedList<ull> *FollowerList::getFollowers(ull userID)
{
    LinkedList<ull> **listPtr = followersMap.get(userID);
    return listPtr ? *listPtr : nullptr;
}

const LinkedList<ull> *FollowerList::getFollowers(ull userID) const
{
    LinkedList<ull> *const *listPtr = followersMap.get(userID);
    return listPtr ? *listPtr : nullptr;
}

size_t FollowerList::followerCount(ull userID) const
{
    LinkedList<ull> *const *listPtr = followersMap.get(userID);

    if (!listPtr || !*listPtr)
        return 0;

    // Use LinkedList's size() method
    return (*listPtr)->size();
}

bool FollowerList::hasFollower(ull userID, ull followerID) const
{
    LinkedList<ull> *const *listPtr = followersMap.get(userID);

    if (!listPtr || !*listPtr)
        return false;

    // Use LinkedList's contains() method
    return (*listPtr)->contains(followerID);
}

ull *FollowerList::findFollowerByUsername(ull userID, const char *username,
                                          const HashMap<ull, const char *> &usernames)
{
    LinkedList<ull> **listPtr = followersMap.get(userID);

    if (!listPtr || !*listPtr)
        return nullptr;

    LinkedList<ull> *list = *listPtr;

    // Linked-list traversal with string comparison
    Node<ull> *current = list->head();

    while (current)
    {
        ull followerID = current->data;
        const char *const *namePtr = usernames.get(followerID);

        if (namePtr && stringEquals(*namePtr, username))
        {
            // Return pointer to the data in the node
            return &(current->data);
        }

        current = current->next;
    }

    return nullptr;
}

void FollowerList::removeUser(ull userID)
{
    LinkedList<ull> **listPtr = followersMap.get(userID);

    if (listPtr && *listPtr)
    {
        delete *listPtr;
    }

    followersMap.remove(userID);
}