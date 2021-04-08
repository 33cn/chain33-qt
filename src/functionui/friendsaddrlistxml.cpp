#include "friendsaddrlistxml.h"
#include "basefuntion.h"

#include <QDomDocument>
#include <QTextStream>
#include <QMessageBox>

FriendsAddrListXml::FriendsAddrListXml()
{
	m_strFileName = GetDefaultDataDir() + "/FriendsAddrList.xml";
}

void FriendsAddrListXml::AddNewData(const QString &strLable, const QString &strAddr)
{
	QList<FriendsAddrData>  AddrDataList;
	ReadXml(AddrDataList);
	for (int i = 0; i < AddrDataList.size(); ++i)
	{
		if (AddrDataList[i].strAddr == strAddr)
			return; // 已存在 不添加
	}

	QFile file(m_strFileName);
	if (!file.open(QFile::ReadOnly))
		return;
	//增加一个一级子节点以及元素
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement();
	QDomElement Item = doc.createElement("Item");
	QDomText text;

	QDomElement lable = doc.createElement("lable");
	text = doc.createTextNode(strLable);
	lable.appendChild(text);
	Item.appendChild(lable);

	QDomElement addr = doc.createElement("addr");
	text = doc.createTextNode(strAddr);
	addr.appendChild(text);
	Item.appendChild(addr);

	root.appendChild(Item);

	if (!file.open(QFile::WriteOnly | QFile::Truncate)) //先读进来，再重写，如果不用truncate就是在后面追加内容，就无效了
		return;
	//输出到文件
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //缩进4格
	file.close();
}

void FriendsAddrListXml::AddNewData(const FriendsAddrData &AddrData)
{
	AddNewData(AddrData.strLable, AddrData.strAddr);
}

bool FriendsAddrListXml::RemoveData(const QString &strLable, const QString &strAddr)
{
	bool Ret = false;
	QFile file(m_strFileName);
	if (false == file.exists())
	{
		return Ret;
	}

	if (!file.open(QFile::ReadOnly))
		return Ret;

	//删除一个一级子节点及其元素，外层节点删除内层节点于此相同
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return Ret;
	}
	file.close();  //一定要记得关掉啊，不然无法完成操作

	QDomElement root = doc.documentElement();
	QDomNodeList list = doc.elementsByTagName("Item"); //由标签名定位
	for (int i = 0; i < list.count(); i++)
	{
		QDomElement e = list.at(i).toElement();
		QDomNodeList listitem = e.childNodes();

		FriendsAddrData ItemAddrData;
		for (int i = 0; i < listitem.count(); i++) //遍历子元素，count和size都可以用,可用于标签数计数
		{
			QDomNode n = listitem.at(i);
			if (n.isElement() && n.nodeName() == "lable")
			{
				ItemAddrData.strLable = n.toElement().text();
			}

			if (n.isElement() && n.nodeName() == "addr")
			{
				ItemAddrData.strAddr = n.toElement().text();
			}
		}

		if (ItemAddrData.strAddr == strAddr && ItemAddrData.strLable == strLable)
		{
			root.removeChild(list.at(i));
			Ret = true;
		}
	}

	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return Ret;
	//输出到文件
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //缩进4格
	file.close();

	return Ret;
}

bool FriendsAddrListXml::RemoveData(const FriendsAddrData &AddrData)
{
	return RemoveData(AddrData.strLable, AddrData.strAddr);
}

void FriendsAddrListXml::UpDataXml(const QString &strOldLable, const QString &strOldAddr, const QString &strNewLable, const QString &strNewAddr)
{
	if (RemoveData(strOldLable, strOldAddr)) {
		AddNewData(strNewLable, strNewAddr);
	}
}

void FriendsAddrListXml::UpDataXml(const FriendsAddrData &OldAddrData, const FriendsAddrData &NewAddrData)
{
	UpDataXml(OldAddrData.strLable, OldAddrData.strAddr, NewAddrData.strLable, NewAddrData.strAddr);
}

bool FriendsAddrListXml::IsExistsAdd(const QString &strLable, const QString &strAddr)
{
	QList<FriendsAddrData>  AddrDataList;
	ReadXml(AddrDataList);
	for (int i = 0; i < AddrDataList.size(); ++i)
	{
		if (strLable == AddrDataList[i].strLable)
		{
			return true;
		}

		if (strAddr == AddrDataList[i].strAddr) // 好友地址不能和我的地址里面有重复，后面再加
		{
			return true;
		}
	}

	return false;
}

bool FriendsAddrListXml::IsExistsAdd(const FriendsAddrData &AddrData)
{
	QList<FriendsAddrData>  AddrDataList;
	ReadXml(AddrDataList);
	for (int i = 0; i < AddrDataList.size(); ++i)
	{
		if (AddrData.strLable == AddrDataList[i].strLable)
		{
			return true;
		}

		if (AddrData.strAddr == AddrDataList[i].strAddr) // 好友地址不能和我的地址里面有重复，后面再加
		{
			return true;
		}
	}

	return false;
}
/*
void FriendsAddrListXml::UpDataXml(const FriendsAddrData &AddrData)
{
	QFile file("test.xml"); //相对路径、绝对路径、资源路径都可以
	if(!file.open(QFile::ReadOnly))
		return;

	//更新一个标签项,如果知道xml的结构，直接定位到那个标签上定点更新
	//或者用遍历的方法去匹配tagname或者attribut，value来更新
	QDomDocument doc;
	if(!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root=doc.documentElement();
	QDomNodeList list=root.elementsByTagName("Item");
	QDomNode node=list.at(list.size()-1).firstChild(); //定位到第三个一级子节点的子元素
	QDomNode oldnode=node.firstChild(); //标签之间的内容作为节点的子节点出现,当前是Pride and Projudice
	node.firstChild().setNodeValue("Emma");
	QDomNode newnode=node.firstChild();
	node.replaceChild(newnode,oldnode);

	if(!file.open(QFile::WriteOnly|QFile::Truncate))
		return;
	//输出到文件
	QTextStream out_stream(&file);
	doc.save(out_stream,4); //缩进4格
	file.close();
}*/

void FriendsAddrListXml::ReadXml(QList<FriendsAddrData> &AddrDataList)
{
	QFile file(m_strFileName);
	if (false == file.exists())
	{
		createXml();
		return;
	}

	if (!file.open(QFile::ReadOnly))
		return;

	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement(); //返回根节点
  //  qDebug()<<root.nodeName();
	QDomNode node = root.firstChild(); //获得第一个子节点
	while (!node.isNull())  //如果节点不空
	{
		if (node.isElement()) //如果节点是元素
		{
			QDomElement e = node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
			QDomNodeList list = e.childNodes();

			FriendsAddrData addrData;
			for (int i = 0; i < list.count(); i++) //遍历子元素，count和size都可以用,可用于标签数计数
			{
				QDomNode n = list.at(i);
				if (node.isElement() && n.nodeName() == "lable")
				{
					addrData.strLable = n.toElement().text();
				}

				if (node.isElement() && n.nodeName() == "addr")
				{
					addrData.strAddr = n.toElement().text();
				}
			}

			if (!addrData.strAddr.isEmpty())
			{
				AddrDataList.push_back(addrData);
			}
		}
		node = node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
	}
}

void FriendsAddrListXml::createXml()
{
	QFile file(m_strFileName); //相对路径、绝对路径、资源路径都可以
	if (!file.open(QFile::WriteOnly | QFile::Truncate)) //可以用QIODevice，Truncate表示清空原来的内容
		return;

	QDomDocument doc;
	QDomProcessingInstruction instruction; //添加处理命令
	instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instruction);
	//添加根节点
	QDomElement root = doc.createElement("FriendsAddrList");
	doc.appendChild(root);

	//输出到文件
	QTextStream out_stream(&file);
	doc.save(out_stream, 4); //缩进4格
	file.close();
}
