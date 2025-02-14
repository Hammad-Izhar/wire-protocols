#pragma once
#include <QLineEdit>
#include <QListWidget>
#include <QWidget>

#include "client/gui/components/spinner.hpp"
#include "models/user.hpp"

class ChatSidebar;


/**
 * @class SearchTab
 * @brief A tab widget for searching users within the chat application.
 *
 * This class provides a search interface where users can enter queries,
 * view search results, and handle success or failure cases.
 */
class SearchTab : public QWidget {
    Q_OBJECT

   public:
   /**
     * @brief Constructs a SearchTab object.
     * @param sidebar A pointer to the ChatSidebar that contains this search tab.
     * @param parent A pointer to the parent QWidget (default is nullptr).
     */
    explicit SearchTab(ChatSidebar* sidebar, QWidget* parent = nullptr);

    /**
     * @brief Resets the search input field and results list.
     */
    void reset();

   private:
    ChatSidebar* sidebar;
    QLineEdit* searchField;
    QListWidget* searchResults;
    Spinner* spinner;

    /**
     * @brief Sets the loading state of the search tab.
     * @param loading Boolean flag indicating whether to show a loading state.
     *
     * This function updates the UI to reflect whether a search process is in progress.
     */
    void set_loading(bool loading);

   private slots:
   /**
     * @brief Initiates the search process based on user input.
     *
     * This function retrieves the input query, performs validation, and triggers the search.
     */
    void search();

     /**
     * @brief Slot triggered when a search is successful.
     * @param accounts A list of user accounts matching the search query.
     *
     * This function updates the search results UI with the retrieved user accounts.
     */
    void onSearchSuccess(const std::vector<User::SharedPtr>& accounts);

    /**
     * @brief Slot triggered when a search fails.
     * @param errorMessage The error message describing the failure reason.
     *
     * This function displays the error message and updates the UI accordingly.
     */
    void onSearchFailure(const QString& errorMessage);
};