#ifndef FRIENDSADDRLISTXML_H
#define FRIENDSADDRLISTXML_H

/*
 * 好友地址信息存在本地 xml 文件中
 */

#include <QList>
#include <QString>
#include <QFile>

struct FriendsAddrData
{
	QString strAddr;
	QString strLable;
};

class FriendsAddrListXml
{
public:
	FriendsAddrListXml();

	void AddNewData(const QString &strLable, const QString &strAddr);
	void AddNewData(const FriendsAddrData &AddrData);
	bool RemoveData(const QString &strLable, const QString &strAddr);
	bool RemoveData(const FriendsAddrData &AddrData);
	void UpDataXml(const QString &strOldLable, const QString &strOldAddr, const QString &strNewLable, const QString &strNewAddr);
	void UpDataXml(const FriendsAddrData &OldAddrData, const FriendsAddrData &NewAddrData);
	void ReadXml(QList<FriendsAddrData> &AddrDataList);

private:
	bool IsExistsAdd(const QString &strLable, const QString &strAddr);  // true is exist, return
	bool IsExistsAdd(const FriendsAddrData &AddrData);
	void createXml();

private:
	QString                 m_strFileName;
};

#endif // FRIENDSADDRLISTXML_H
