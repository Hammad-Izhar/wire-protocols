#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

/**
 * @class RegistrationTab
 * @brief A tab widget responsible for handling user registration.
 *
 * This class provides UI components and logic for user registration within a tabbed interface.
 * It includes input fields, validation, and handles registration success or failure events.
 */
class RegistrationTab : public QWidget {
    Q_OBJECT
   public:
   /**
     * @brief Constructs a RegistrationTab object.
     * @param tabWidget A pointer to the parent QTabWidget that contains this tab.
     * @param parent A pointer to the parent QWidget (default is nullptr).
     */
    explicit RegistrationTab(QTabWidget* tabWidget, QWidget* parent = nullptr);

    /**
     * @brief Resets the registration form fields and state.
     */
    void reset();

   private slots:
   /**
     * @brief Slot triggered when registration is successful.
     *
     * This function handles UI updates and any post-registration logic upon success.
     */
    void onRegistrationSuccess();

    /**
     * @brief Slot triggered when registration fails.
     * @param errorMessage The error message describing the failure reason.
     *
     * This function displays the error message and updates the UI accordingly.
     */
    void onRegistrationFailure(const QString& errorMessage);

   private:
    QGroupBox* inputGroup;
    QTabWidget* tabWidget;
    Spinner* spinner;

    /**
     * @brief Handles the submission of the registration form.
     *
     * This function gathers input data, validates it, and initiates the registration process.
     */
    void on_submit();

    /**
     * @brief Sets the loading state of the registration form.
     * @param isLoading Boolean flag indicating whether to show a loading state.
     *
     * This function updates the UI to reflect whether a registration process is in progress.
     */
    void set_loading(bool isLoading);

    /**
     * @brief Creates a validation function for text input.
     * @param validator A pointer to a QValidator used for validation.
     * @return A function that validates a QString based on the provided validator.
     *
     * This static function returns a callable that validates text input using the given validator.
     */
    static std::function<bool(const QString&, QWidget*)> validate_text(const QValidator* validator);
};