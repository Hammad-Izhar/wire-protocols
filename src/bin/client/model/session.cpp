#include "client/model/session.hpp"

Session &Session::getInstance()
{
    static Session instance;
    return instance;
}
