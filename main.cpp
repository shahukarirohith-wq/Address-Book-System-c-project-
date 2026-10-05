#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

const std::string FILE_NAME = "contacts.txt";

class Contact {
private:
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    std::string city;
    std::string state;
    std::string pin;

public:
    Contact() = default;

    Contact(const std::string& contactName,
            const std::string& contactPhone,
            const std::string& contactEmail,
            const std::string& contactAddress,
            const std::string& contactCity,
            const std::string& contactState,
            const std::string& contactPin)
        : name(contactName), phone(contactPhone), email(contactEmail),
          address(contactAddress), city(contactCity), state(contactState),
          pin(contactPin) {
    }

    const std::string& getName() const { return name; }
    const std::string& getPhone() const { return phone; }
    const std::string& getEmail() const { return email; }
    const std::string& getAddress() const { return address; }
    const std::string& getCity() const { return city; }
    const std::string& getState() const { return state; }
    const std::string& getPin() const { return pin; }
};

std::string toLower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char character) { return std::tolower(character); });
    return result;
}

bool containsOnlyDigits(const std::string& text) {
    return !text.empty() && std::all_of(text.begin(), text.end(),
        [](unsigned char character) { return std::isdigit(character); });
}

bool isValidPhone(const std::string& phone) {
    return phone.length() >= 10 && phone.length() <= 15 && containsOnlyDigits(phone);
}

bool isValidEmail(const std::string& email) {
    const std::size_t at = email.find('@');
    const std::size_t dot = email.rfind('.');
    return at != std::string::npos && dot != std::string::npos && at > 0 && dot > at + 1 &&
           dot < email.length() - 1;
}

bool isValidPin(const std::string& pin) {
    return pin.length() >= 4 && pin.length() <= 10 && containsOnlyDigits(pin);
}

std::string readNonEmpty(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);

        if (!value.empty()) {
            return value;
        }
        std::cout << "This field cannot be empty. Please try again.\n";
    }
}

std::string readPhone() {
    std::string phone;
    while (true) {
        phone = readNonEmpty("Phone (10 to 15 digits): ");
        if (isValidPhone(phone)) {
            return phone;
        }
        std::cout << "Enter only 10 to 15 digits.\n";
    }
}

std::string readEmail() {
    std::string email;
    while (true) {
        email = readNonEmpty("Email: ");
        if (isValidEmail(email)) {
            return email;
        }
        std::cout << "Enter an email such as name@example.com.\n";
    }
}

std::string readPin() {
    std::string pin;
    while (true) {
        pin = readNonEmpty("PIN / ZIP (4 to 10 digits): ");
        if (isValidPin(pin)) {
            return pin;
        }
        std::cout << "Enter only 4 to 10 digits.\n";
    }
}

Contact readContactDetails() {
    std::cout << "\nEnter contact details\n";
    const std::string name = readNonEmpty("Name: ");
    const std::string phone = readPhone();
    const std::string email = readEmail();
    const std::string address = readNonEmpty("Address: ");
    const std::string city = readNonEmpty("City: ");
    const std::string state = readNonEmpty("State: ");
    const std::string pin = readPin();

    return Contact(name, phone, email, address, city, state, pin);
}

void printLine(char character = '-') {
    std::cout << std::string(124, character) << '\n';
}

void printContactTableHeader() {
    printLine();
    std::cout << std::left
              << std::setw(4) << "No."
              << std::setw(20) << "Name"
              << std::setw(16) << "Phone"
              << std::setw(28) << "Email"
              << std::setw(18) << "City"
              << std::setw(18) << "State"
              << std::setw(10) << "PIN" << '\n';
    printLine();
}

void printContactRow(const Contact& contact, std::size_t number) {
    std::cout << std::left
              << std::setw(4) << number
              << std::setw(20) << contact.getName().substr(0, 19)
              << std::setw(16) << contact.getPhone().substr(0, 15)
              << std::setw(28) << contact.getEmail().substr(0, 27)
              << std::setw(18) << contact.getCity().substr(0, 17)
              << std::setw(18) << contact.getState().substr(0, 17)
              << std::setw(10) << contact.getPin().substr(0, 9) << '\n';
}

void displayContactDetails(const Contact& contact) {
    std::cout << "\nName: " << contact.getName() << '\n';
    std::cout << "Phone: " << contact.getPhone() << '\n';
    std::cout << "Email: " << contact.getEmail() << '\n';
    std::cout << "Address: " << contact.getAddress() << '\n';
    std::cout << "City: " << contact.getCity() << '\n';
    std::cout << "State: " << contact.getState() << '\n';
    std::cout << "PIN / ZIP: " << contact.getPin() << '\n';
}

void displayAll(const std::vector<Contact>& contacts) {
    if (contacts.empty()) {
        std::cout << "\nNo contacts saved yet.\n";
        return;
    }

    std::cout << "\nAll Contacts\n";
    printContactTableHeader();
    for (std::size_t index = 0; index < contacts.size(); ++index) {
        printContactRow(contacts[index], index + 1);
    }
    printLine();
}

void saveContacts(const std::vector<Contact>& contacts) {
    std::ofstream file(FILE_NAME);
    if (!file) {
        std::cout << "Could not save contacts to " << FILE_NAME << ".\n";
        return;
    }

    // quoted() preserves spaces and quotation marks inside every text field.
    for (const Contact& contact : contacts) {
        file << std::quoted(contact.getName()) << ' '
             << std::quoted(contact.getPhone()) << ' '
             << std::quoted(contact.getEmail()) << ' '
             << std::quoted(contact.getAddress()) << ' '
             << std::quoted(contact.getCity()) << ' '
             << std::quoted(contact.getState()) << ' '
             << std::quoted(contact.getPin()) << '\n';
    }
}

void loadContacts(std::vector<Contact>& contacts) {
    std::ifstream file(FILE_NAME);
    if (!file) {
        return; // A first-time user will not have a contacts file yet.
    }

    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    std::string city;
    std::string state;
    std::string pin;

    while (file >> std::quoted(name) >> std::quoted(phone) >> std::quoted(email) >>
           std::quoted(address) >> std::quoted(city) >> std::quoted(state) >>
           std::quoted(pin)) {
        contacts.emplace_back(name, phone, email, address, city, state, pin);
    }
}

void addContact(std::vector<Contact>& contacts) {
    contacts.push_back(readContactDetails());
    saveContacts(contacts);
    std::cout << "Contact added successfully.\n";
}

std::vector<std::size_t> findContacts(const std::vector<Contact>& contacts,
                                      const std::string& key) {
    std::vector<std::size_t> matches;
    const std::string searchKey = toLower(key);

    for (std::size_t index = 0; index < contacts.size(); ++index) {
        const Contact& contact = contacts[index];
        if (toLower(contact.getName()).find(searchKey) != std::string::npos ||
            contact.getPhone() == key ||
            toLower(contact.getEmail()) == searchKey) {
            matches.push_back(index);
        }
    }
    return matches;
}

void searchContact(const std::vector<Contact>& contacts) {
    if (contacts.empty()) {
        std::cout << "\nNo contacts saved yet.\n";
        return;
    }

    const std::string key = readNonEmpty("\nSearch by name, phone, or email: ");
    const std::vector<std::size_t> matches = findContacts(contacts, key);

    if (matches.empty()) {
        std::cout << "Contact not found.\n";
        return;
    }

    std::cout << "\nSearch Results\n";
    printContactTableHeader();
    for (std::size_t index : matches) {
        printContactRow(contacts[index], index + 1);
    }
    printLine();
}

int chooseMatch(const std::vector<std::size_t>& matches) {
    if (matches.size() == 1) {
        return static_cast<int>(matches[0]);
    }

    while (true) {
        const std::string answer = readNonEmpty("Enter the contact number to select: ");
        if (containsOnlyDigits(answer)) {
            const std::size_t number = static_cast<std::size_t>(std::stoul(answer));
            for (std::size_t index : matches) {
                if (number == index + 1) {
                    return static_cast<int>(index);
                }
            }
        }
        std::cout << "Choose one of the contact numbers shown above.\n";
    }
}

int findOneContact(const std::vector<Contact>& contacts) {
    if (contacts.empty()) {
        std::cout << "\nNo contacts saved yet.\n";
        return -1;
    }

    const std::string key = readNonEmpty("\nEnter a name, phone, or email: ");
    const std::vector<std::size_t> matches = findContacts(contacts, key);
    if (matches.empty()) {
        std::cout << "Contact not found.\n";
        return -1;
    }

    std::cout << "\nMatching Contacts\n";
    printContactTableHeader();
    for (std::size_t index : matches) {
        printContactRow(contacts[index], index + 1);
    }
    printLine();
    return chooseMatch(matches);
}

void updateContact(std::vector<Contact>& contacts) {
    const int selectedIndex = findOneContact(contacts);
    if (selectedIndex == -1) {
        return;
    }

    std::cout << "\nCurrent details:\n";
    displayContactDetails(contacts[selectedIndex]);
    std::cout << "\nEnter replacement details.\n";
    contacts[selectedIndex] = readContactDetails();
    saveContacts(contacts);
    std::cout << "Contact updated successfully.\n";
}

void deleteContact(std::vector<Contact>& contacts) {
    const int selectedIndex = findOneContact(contacts);
    if (selectedIndex == -1) {
        return;
    }

    std::cout << "\nSelected contact:\n";
    displayContactDetails(contacts[selectedIndex]);
    const std::string confirmation = readNonEmpty("Delete this contact? (y/n): ");

    if (toLower(confirmation) == "y" || toLower(confirmation) == "yes") {
        contacts.erase(contacts.begin() + selectedIndex);
        saveContacts(contacts);
        std::cout << "Contact deleted successfully.\n";
    } else {
        std::cout << "Deletion cancelled.\n";
    }
}

void sortContacts(std::vector<Contact>& contacts) {
    std::sort(contacts.begin(), contacts.end(), [](const Contact& first, const Contact& second) {
        return toLower(first.getName()) < toLower(second.getName());
    });
    saveContacts(contacts);
    std::cout << "Contacts sorted alphabetically by name.\n";
}

int readMenuChoice() {
    while (true) {
        const std::string choice = readNonEmpty("Enter your choice: ");
        if (containsOnlyDigits(choice)) {
            try {
                const int number = std::stoi(choice);
                if (number >= 1 && number <= 7) {
                    return number;
                }
            } catch (const std::out_of_range&) {
                // The message below handles a number that is too large for int.
            }
        }
        std::cout << "Enter a number from 1 to 7.\n";
    }
}

void displayMenu() {
    std::cout << "\n================================\n";
    std::cout << "         ADDRESS BOOK SYSTEM\n";
    std::cout << "================================\n";
    std::cout << "1. Add Contact\n";
    std::cout << "2. Display All Contacts\n";
    std::cout << "3. Search Contact\n";
    std::cout << "4. Update Contact\n";
    std::cout << "5. Delete Contact\n";
    std::cout << "6. Sort Contacts by Name\n";
    std::cout << "7. Exit\n";
}

int main() {
    std::vector<Contact> contacts;
    loadContacts(contacts);
    std::cout << "Loaded " << contacts.size() << " contact(s).\n";

    while (true) {
        displayMenu();
        const int choice = readMenuChoice();

        switch (choice) {
            case 1: addContact(contacts); break;
            case 2: displayAll(contacts); break;
            case 3: searchContact(contacts); break;
            case 4: updateContact(contacts); break;
            case 5: deleteContact(contacts); break;
            case 6: sortContacts(contacts); break;
            case 7:
                saveContacts(contacts);
                std::cout << "Contacts saved. Goodbye!\n";
                return 0;
        }
    }
}
