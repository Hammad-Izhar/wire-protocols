#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

/**
 * @brief ConnectionWindow is a QWidget that allows the user to input a server
 * address and port number.
 * 
 * The user can submit the connection information to the server by clicking the
 * submit button.
 */
class ConnectionWindow : public QWidget {
    Q_OBJECT
   public:
   /**
    * @brief Construct a new ConnectionWindow object.
    */
    explicit ConnectionWindow(QWidget* parent = nullptr);

    void reset();

   private:
    QGroupBox* inputGroup;
    Spinner* spinner;

    void on_submit();

    void set_loading(bool isLoading);

    std::function<bool(const QString&, QWidget*)> validate_text(const QValidator* validator);
};