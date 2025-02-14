# Sock-et Out
## COMPSCI 2620 Design Exercise 1: Wire Protocols
### Group 36: Hammad Izhar, Thomas Kaminsky

Below is a summary of the important components

Sock-et Out is an SMS-like chat service enabling users to communicate in multi-person channels in real-time. 


## Table of Contents

### [Setup and Running](#setup-and-running-1)

### [Overview of Functionality](#overview-of-functionality-1)

### [Classes](#classes-1)
* **[UUID](#uuid)**
* **[Snowflake](#snowflake)**
* **[User](#user)**
* **[Message](#message)**
* **[Channel](#channel)**

### [Data Management](#data-management-1)
* **[Database](#database)**
* **[Message Table](#message-table)**
* **[Channel Table](#channel-table)**
* **[Password Table](#password-table)**
* **[User Table](#user-table)**

### [Messages](#messages-1)
* **[Header](#header)**
* **[Register Account](#register-account)**
* **[Login](#login)**
* **[List Accounts](#list-accounts)**
* **[Create Channel](#create-channel)**
* **[Send Message](#send-message)**
* **[Delete Message](#delete-message)**
* **[Delete Account](#delete-account)**









### [Server](#server)








# Setup and Running

To run the code, you'll need Docker and an X11 server. On Linux, you most likely already have an X11 server installed. On Mac/Windows consider using [XQuartz](https://www.xquartz.org/) or [Xming](https://sourceforge.net/projects/xming/), respectively.

**NOTE** This code has only been verified on Mac, and briefly tested on Linux. Your mileage may very.

>[!INFO]
> On Linux, `docker.host.internal` is not natively supported. Therefore, it is recommended to first verify the value of `$DISPLAY` locally and then export it in the docker container. Also be sure to verify xhost connections are allowed by `xhost +local:`

Open XQuartz. In 'XQuartz' settings, navigate Settings -> Security -> 'Allow connections from network clients', and then restart machine.

```bash
git clone https://github.com/Hammad-Izhar/wire-protocols && cd wire-protocols
docker compose up --build -d
docker attach wire-protocols-app-1 # replace this with the container name
```

To build the code:

```
cd ~/cs2620/wire-protocols/build
cmake ..
make
```

If you're getting error `Authorization required, but no authorization protocol specified`, open a new terminal and run

```
xhost +localhost
```




# Overview of Functionality


# Classes

Below we describe the classes used for comprehending messages, users, and channels. 

In general, class headers are initialized in the `/include/` directory, and implementations in the `/src/` directory. The full project structure is outlined below:

```
.
├── assets
│   └── [Files including profile pictures, loading gifs, etc.]
├── include
|   |   [Contains headers for all relevant classes.]
│   ├── client
│   │   ├── gui
│   │   │   └── [Files for displaying the app]
│   │   └── model
│   │       └── [General classes for the client]
│   ├── constants.hpp [Relevant constants]
│   ├── json.hpp [For JSON serialization/deserialization]
│   ├── message
│   │   └── [Classes for sending messages between client and server]
│   ├── models
│   │   └── [General-use classes for comprehending data]
│   └── server
│       ├── db
│       │   └── [Classes for data storage and access]
│       └── model
│           └── [Classes for server-side message-passing]
├── src
│   ├── bin
│   │   ├── client
│   │   │   ├── gui
│   │   │   │   └── [Files for displaying the app]
│   │   │   ├── main.cpp [Main file for running client]
│   │   │   └── model
│   │   │       └── [General classes for the client]
│   │   └── server
│   │       ├── db
│   │       │   └── [Classes for data storage and access]
│   │       ├── main.cpp [Main file for running server]
│   │       └── model
│   │           └── [Classes for server-side message-passing]
│   ├── message
│   │   └── [Classes for sending messages between client and server]
│   └── models
│       └── [General-use classes for comprehending data]
└── test
    ├── main.cpp [Main file for running tests]
    ├── message
    │   └── [Tests for sending/receiving messages]
    └── models
        └── [Tests for individual backend components]
```

## UUID

To accomodate a large user base, we created a class for unique user IDs to easily store large integers as an `std::array<uint8_t, 16>`. Values are either randomly generated on initialization or pre-populated. Otherwise, it's pretty spare&mdash;methods to serialize/deserialize UUIDs, perform numeric comparisons, and so on.

## Snowflake

Snowflake is a utility class for generating unique snowflakes for each message. See `src/models/snowflake.cpp` and `src/models/message.cpp` for details.

## User

`User`s share important profile information for individuals signed up for the service.


* `UUID uid`: The unique user ID used for referencing users in the database
* `std::string username`: Their username, for login.
* `std::string display_name`: Their display name for messages.
* `std::string profile_pic`: A link to the user's profile picture.
* `std::string public_key`: The user's public key (for message encryption).
* `std::vector<UUID> channels`: A vector of UUIDs referencing all channels of which the user is a part.

Otherwise, methods are getters/setters for the relevant quantities above. 

## Message

`Message`s contain user communications, as well as related metadata:

* `uint64_t snowflake`: Unique ID for each message.
* `UUID sender_id`: ID of message sender.
* `UUID channel_id`: ID of the channel that the message was sent in.
* `uint64_t created_at`: Time of message creation.
* `uint64_t modified_at`: Time that the message was last modified.
* `std::vector<UUID> read_by`: A list of all users who have read the message.
* `std::string text`: The message payload.



## Channel

Each `Channel` stores references to all users and messages that it contains, as well as chanel metadata:

* `UUID uid`: The channel's unique identifier, for referencing it.
* `std::vector<UUID> user_uids`: A list of IDs corresponding to all users belonging to the channel.
* `std::vector<uint64_t> message_snowflakes`: Message snowflakes corresponding to each message in the channel.

Methods are just serialize/deserialize and relevant getters/setters.

# Data Management

Below we describe the important methods for managing our app's data, implemented in `server/db`.

## Database

Our `Database` is structured somewhat like a poor man's SQL database. It serves as the point of entry for any updates to channel state, including adding/removing users, messages, and channels, creating and logging into accounts, and searching for users by regex. It's composed of four dictionaries:

* `unique_ptr<UserTable> users`: The user database.
* `unique_ptr<MessageTable> messages`: The message database.
* `unique_ptr<ChannelTable> channels`: The channel database.
* `unique_ptr<PasswordTable passwords`: The password database.

The database is responsible for implementing higher-order operations between these databases; e.g. when removing a user, we must remove the user from the user table, remove all of their messages from the message table, remove them from each of their channels, and delete the channel if they were the only member. 

## Message Table

The message table is comprised of an `std::unordered_map<uint64_t, Message::SharedPtr> data` containing all messages, referenced by snowflake. Functionality consists of simple getters/setters.

## User Table

Like the message table, the user table is an ` std::unordered_map<UUID, User::SharedPtr>` of users which implements basic getters/setters. The only additions are two methods:

* `std::vector<UUID> get_uuids_matching_regex(std::string regex)`: Grabs the IDs of all users whose username matches a properly-formatted regex (for name lookup).
* `std::optional<UUID> get_uid_from_username(std::string username)`: Gets the UUID associated with a username, if it exists. Used for verifying that usernames are unique during registration.

## Channel Table

Another dictionary from IDs to channels (`std::unordered_map<UUID, Channel::SharedPtr>`) implementing getters/setters.

## Password Table

Like the other tables, a `std::unordered_map<UUID, std::pair<std::string, std::string>>`. This one maps from user IDs to hashed passwords and a string corresponding to [salt](https://en.wikipedia.org/wiki/Salt_(cryptography)), used to make the hash more secure. Includes methods to add/remove passwords, as well as a function `std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password)` which checks whether a user has supplied the right password to log in.



# Messages 

This section describes the messages sent between client and server to control make requests for each database action.

All messages implement serialize / deserialize, both with JSON and our custom serialization scheme.

All messages are routed to separate handlers in `client/models/message_handlers.hpp/cpp` and `server/models/message_handlers.hpp/cpp`.

## Header

The header is prepended to all messages, and it specifies metadata:

*  `uint8_t version`: Currently, 1.
* `enum Operation operation`: The message being sent (see the `enum` in `include/message/header.hpp`)
* `uint16_t packet_length`: Size of the payload.

## Register Account

`Client -> Server`

Sends username, password, and display name.

Handler tries to create an account, with success if username is not taken and password is valid.

**Response**

`Server -> Client`

Sends server response back to client.

Handler either redirects to login (on success) or notes an error during account creation.

## Login

`Client -> Server`

Sends username and password. 

Handler checks if client can accept new logins, and, if so, checks user credentials. On success, the client authenticates a user (setting a flag that the user is logged in). Otherwise, login fails.

**Response**

`Server -> Client`

Sends server response. Transitions to logged in screen if successful.

## List Accounts

`Client -> Server`

Sends a regex for pattern-matching users.

Handler grabs all accounts matching the regex by UUID, and sends copies of the users back to the client.

**Response**

`Server -> Client`

Response sends the users back to client, handling when regex fails to receive any.

## Create Channel

`Client -> Server`

Sends the channel name and a list of initial member IDs. 

Handler creates the channel and returns a pointer to it if possible.

**Response**

`Server -> Client`

Return message sends a shared pointer to the channel or shares the error.


## Send Message

## Delete Message

## Delete Account







# Server

















