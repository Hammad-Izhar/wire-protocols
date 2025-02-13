#include <QLineEdit>
#include <QWidget>

#include "client/gui/components/spinner.hpp"
#include "models/user.hpp"

class SearchTab : public QWidget {
    Q_OBJECT

   public:
    explicit SearchTab(QWidget* parent = nullptr);

   private:
    QLineEdit* searchField;
    Spinner* spinner;
    void set_loading(bool loading);

   private slots:
    void search();
    void onSearchSuccess(const std::vector<User::SharedPtr>& accounts);
    void onSearchFailure(const QString& errorMessage);
};