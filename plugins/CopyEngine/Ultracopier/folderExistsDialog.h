/** \file folderExistsDialog.h
\brief Define the dialog when file exists
\author alpha_one_x86
\licence GPL3, see the file COPYING */

#ifndef FOLDERISSAMEDIALOG_H
#define FOLDERISSAMEDIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <QString>
#include <QDateTime>
#include <QDir>

#include "Environment.h"

namespace Ui {
    class folderExistsDialog;
}

/// \brief to show file exists dialog, and ask what do
class folderExistsDialog : public QDialog
{
    Q_OBJECT

public:
	/// \brief create the object and pass all the informations to it
	explicit folderExistsDialog(QWidget *parent,QFileInfo source,bool isSame,QFileInfo destination,QString firstRenamingRule,QString otherRenamingRule);
	~folderExistsDialog();
	/// \brief return the the always checkbox is checked
	bool getAlways();
	/// \brief return the action clicked
	FolderExistsAction getAction();
	/// \brief return the new rename is case in manual renaming
	QString getNewName();
protected:
	void changeEvent(QEvent *e);
private slots:
	void on_SuggestNewName_clicked();
	void on_Rename_clicked();
	void on_Skip_clicked();
	void on_Cancel_clicked();
	void on_Merge_clicked();
private:
	Ui::folderExistsDialog *ui;
	FolderExistsAction action;
	QString oldName;
	QString firstRenamingRule;
	QString otherRenamingRule;
	QFileInfo destinationInfo;
};

#endif // FOLDERISSAMEDIALOG_H
