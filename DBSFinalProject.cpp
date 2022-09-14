/* ****************
Khoi Vu 124611203
DBS211.NGG
Project Final Submission
**************** */

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <occi.h>
#include <cstring>
#include <iomanip>
using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct Employee {
	int m_employeeNumber;
	char m_lastName[50];
	char m_firstName[50];
	char m_email[100];
	char m_phone[50];
	char m_extension[10];
	char m_reportsTo[100];
	char m_jobTitle[50];
	char m_city[50];
	void displayEmployee();
};
int getInt(int min, int max, const char* prompt = nullptr) {
	int input;
	if (prompt != nullptr) {
		cout << prompt;
	}
	cin >> input;
	do {
		if (cin.fail() || input < min || input > max) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "You have not entered a valid number, please re-enter: ";
			cin >> input;
		}
	} while (cin.fail());
	return input;
}
int menu(void) {
	int input;
	cout << endl;
	cout << "********************* HR Menu *********************" << endl;
	cout << "1)	Find Employee\n2)	Employees Report\n3)	Add Employee\n4)	Update Employee\n5)	Remove Employee\n0)	Exit" << endl;
	cout << "Please enter a menu selection: ";
	input = getInt(0, 5);
	return input;
}
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp) {
	string query;
	string lastName, firstName, extension, email, phone, reportsTo, jobTitle, city;
	int found;
	Statement* stmt = conn->createStatement();
	query = "SELECT e.employeenumber, e.lastname, e.firstname, e.email,  o.phone, e.extension, e.jobtitle, em.firstname || em.lastname AS managerName, o.city FROM employees e	LEFT OUTER JOIN employees em ON e.reportsto = em.employeenumber	LEFT OUTER JOIN offices o ON e.officecode = o.officecode WHERE e.employeenumber =" + to_string(employeeNumber) + "ORDER BY e.employeenumber";
	ResultSet* rs = stmt->executeQuery(query);
	while (rs->next()) {
		emp->m_employeeNumber = rs->getInt(1);
		lastName = rs->getString(2);
		strcpy(emp->m_lastName, lastName.c_str());
		firstName = rs->getString(3);
		strcpy(emp->m_firstName, firstName.c_str());
		email = rs->getString(4);
		strcpy(emp->m_email, email.c_str());
		phone = rs->getString(5);
		strcpy(emp->m_phone, phone.c_str());
		extension = rs->getString(6);
		strcpy(emp->m_extension, extension.c_str());
		jobTitle = rs->getString(7);
		strcpy(emp->m_jobTitle, jobTitle.c_str());
		reportsTo = rs->getString(8);
		strcpy(emp->m_reportsTo, reportsTo.c_str());
		city = rs->getString(9);
		strcpy(emp->m_city, city.c_str());
	}
	if (lastName[0] != '\0') {
		found = 1;
	}
	else {
		found = 0;
	}
	return found;
}
void Employee::displayEmployee() {
	cout << endl;
	cout << "employeeNumber = " << m_employeeNumber << endl;
	cout << "lastName = " << m_lastName << endl;
	cout << "firstName = " << m_firstName << endl;
	cout << "email = " << m_email << endl;
	cout << "phone = " << m_phone << endl;
	cout << "extension = " << m_extension << endl;
	cout << "reportsTo = " << m_reportsTo << endl;
	cout << "jobTitle = " << m_jobTitle << endl;
	cout << "city = " << m_city << endl;
}
void displayEmployee(Connection* conn, struct Employee emp) {
	int employeeInput; int employeeFound;
	try {
		cout << "Please enter an employee number to search: ";
		employeeInput = getInt(0, 9999);
		employeeFound = findEmployee(conn, employeeInput, &emp);
		if (employeeFound == 0) {
			cout << "Employee " << employeeInput << " does not exist." << endl;
		}
		else {
			emp.displayEmployee();
		}
	}
	catch (BatchSQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
}
void displayAllEmployees(Connection* conn) {
	string query;
	int employeeNumber;
	string fullName, email, phone, extension, reportsTo;
	int count = 0;
	try {
		Statement* stmt = conn->createStatement();
		query = "SELECT e.employeenumber, e.firstname || ' ' || e.lastname AS fullName,  e.email,  o.phone, e.extension, em.firstname || ' ' || em.lastname AS managerName FROM employees e	LEFT OUTER JOIN employees em ON e.reportsto = em.employeenumber	LEFT OUTER JOIN offices o ON e.officecode = o.officecode ORDER BY e.employeenumber";
		ResultSet* rs = stmt->executeQuery(query);
		cout << endl;
		cout << "E       Employee Name 	       Email 				  Phone               Ext       Manager" << endl;
		cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
		while (rs->next()) {
			employeeNumber = rs->getInt(1);
			fullName = rs->getString(2);
			email = rs->getString(3);
			phone = rs->getString(4);
			extension = rs->getString(5);
			reportsTo = rs->getString(6);
			count++;
			cout << left;
			cout << setw(8) << employeeNumber;
			cout << setw(23) << fullName;
			cout << setw(35) << email;
			cout << setw(20) << phone;
			cout << setw(10) << extension;
			cout << reportsTo << endl;
		}
		if (count == 0) {
			cout << "There is no employees’ information to be displayed." << endl;
		}
	}
	catch (BatchSQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
}
string convertCharArrayToString(char* charArray) {
	string s = charArray;
	return s;
}
void insertEmployee(Connection* conn, struct Employee emp) {
	int foundExisting;
	int officeCode;
	string query;
	string lastName, firstName, extension, email, phone, reportsTo, jobTitle, city;
	try {
		cout << "Please enter the new employee's information below." << endl;
		cout << "Employee Number: ";
		cin >> emp.m_employeeNumber;
		cout << "Last Name: ";
		cin.ignore();
		cin.get(emp.m_lastName, '\n');
		cin.ignore();
		cout << "First Name: ";
		cin.get(emp.m_firstName, '\n');
		cin.ignore();
		cout << "Extension: ";
		cin.get(emp.m_extension, '\n');
		cin.ignore();
		cout << "Email: ";
		cin >> emp.m_email;
		cout << "Job Title: ";
		cin.ignore();
		cin.get(emp.m_jobTitle, '\n');
		cin.ignore();
		cout << "City: ";
		cin.get(emp.m_city, '\n');
		cin.ignore();
		strcpy(emp.m_phone, "+1 6560 219 4782");
		strcpy(emp.m_reportsTo, "1002");
		officeCode = 1;
		foundExisting = findEmployee(conn, emp.m_employeeNumber, &emp);
		if (foundExisting) {
			cout << "An employee with the same employee number exists." << endl;
		}
		else {
			Statement* stmt = conn->createStatement();
			query = "INSERT INTO employees (employeenumber, lastname, firstname, extension, email, officecode, reportsto, jobtitle) VALUES (" + to_string(emp.m_employeeNumber) + ", \'" + convertCharArrayToString(emp.m_lastName) + "\', \'" + convertCharArrayToString(emp.m_firstName) + "\', \'" + convertCharArrayToString(emp.m_extension) + "\', \'" + convertCharArrayToString(emp.m_email) + "\', " + to_string(officeCode) + ", \'" + convertCharArrayToString(emp.m_reportsTo) + "\', \'" + convertCharArrayToString(emp.m_jobTitle) + "\')";
			ResultSet* rs = stmt->executeQuery(query);
			cout << "The new employee is added successfully.";
			ResultSet* re = stmt->executeQuery("commit");
		}
	}
	catch (BatchSQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
}
void updateEmplyee(Connection* conn, int employeenumber) {
	Employee emp{};
	char extension[10] = "";
	string query;
	int found;
	found = findEmployee(conn, employeenumber, &emp);
	if (found) {
		cout << "New Extension: ";
		cin.ignore(1000, '\n');
		cin.get(extension, '\n');
		cin.ignore();
		try {
			Statement* stmt = conn->createStatement();
			query = "UPDATE employees SET extension=\'" + convertCharArrayToString(extension) + "\' WHERE employeenumber=" + to_string(employeenumber);
			ResultSet* rs = stmt->executeQuery(query);
			cout << "The employee extension updated successfully." << endl;
			ResultSet* re = stmt->executeQuery("commit");

		}
		catch (BatchSQLException& sqlExcp) {
			cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		}
	}
	else {
		cout << "There is no employee by that ID";
	}
}
void removeEmployee(Connection* conn, int employeenumber) {
	bool valid = false;
	char confirm{};
	string query;
	Employee emp{};
	int foundExisting;
	foundExisting = findEmployee(conn, employeenumber, &emp);
	if (foundExisting) {
		cout << "\nYou are removing employee:";
		emp.displayEmployee();
		cout << "Enter Y to confirm or N to cancel: ";
		do {
			cin >> confirm;
			if (confirm == 'Y' || confirm == 'y') {
				valid = true;
				try {
					Statement* stmt = conn->createStatement();
					query = "DELETE FROM employees WHERE employeeNumber=" + to_string(employeenumber);
					ResultSet* rs = stmt->executeQuery(query);
					cout << "\nRemove employee successfully." << endl;
					ResultSet* re = stmt->executeQuery("commit");
				}
				catch (BatchSQLException& sqlExcp) {
					cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
				}
			}
			else if (confirm == 'N' || confirm == 'n') {
				valid = true;
			}
			else {
				cout << "Invalid data, retry: ";
			}

		} while (valid == false);
	}
}
int main(void) {
	Employee emp{};
	// OCCI Variables
	Environment* env = nullptr;
	Connection* conn = nullptr;
	// User Variables
	string usr = "dbs211_221g18";	// this is your login assigned to you
	string pass = "54127718";	// this is your password assinged to you
	string srv = "myoracle12c.senecacollege.ca:1521/oracle12c";
	int menuSelection;
	int done = 0;
	int employeenumber = 0;
	string query;
	try {
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(usr, pass, srv);
		cout << "Connection is Successful" << endl;

		//Statement* stmt = conn->createStatement();
		do {
			menuSelection = menu();
			switch (menuSelection) {
			case 0:
				done = 1;
				break;
			case 1:
				displayEmployee(conn, emp);
				break;
			case 2:
				displayAllEmployees(conn);
				break;
			case 3:
				insertEmployee(conn, emp);
				break;
			case 4:
				employeenumber = getInt(0, 9999, "Employee Number: ");
				updateEmplyee(conn, employeenumber);
				break;
			case 5:
				employeenumber = getInt(0, 9999, "Employee Number: ");
				removeEmployee(conn, employeenumber);
				break;
			default:
				// do nothing;
				break;
			}
		} while (done != 1);
		/*conn->terminateStatement(stmt);*/
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (BatchSQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return 0;
}