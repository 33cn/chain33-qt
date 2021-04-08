#ifndef ENUMTYPE_H
#define ENUMTYPE_H

/*
 * 宏定义 枚举等头文件
 */

#include <QString>

#define TYPESENDING         "Sending"
#define TYPERECEIVEING      "Receiveing"

#ifdef WIN32
#define Tabel_Item_W        32
#else
#define Tabel_Item_W        30
#endif

#define le8  100000000.0
#define le6  1000000.0
#define le5  100000
#define le3  1000
#define lem7 0.000001
#define lem10 0.0000000001

/** General change type (added, updated, removed). */
enum ChangeType
{
    CT_NEW,
    CT_UPDATED,
    CT_DELETED
};

enum AddrTypeTabs {
    TabsReceiving = 0,      // 我的地址
    TabsSending = 1         // 好友地址
};

enum AddrModeType {
    ForSending, /**< Open address book to pick address for sending */
    ForEditing  /**< Open address book for editing */
};

enum RoleIndex {
    TypeRole = Qt::UserRole /**< Type of address (#Send or #Receive) */
};

/** Return status of edit/insert operation */
enum EditStatus {
    EditStatus_OK,                     /**< Everything ok */
    EditStatus_NO_CHANGES,             /**< No changes were made during edit operation */
    EditStatus_INVALID_ADDRESS,        /**< Unparseable address */
    EditStatus_DUPLICATE_ADDRESS,      /**< Address already in address book */
    EditStatus_WALLET_UNLOCK_FAILURE,  /**< Wallet could not be unlocked to create new receiving address */
    EditStatus_KEY_GENERATION_FAILURE  /**< Generating a new public key for a receiving address failed */
};

enum EditAddressDialogMode {
    NewReceivingAddress,
    NewSendingAddress,
    EditReceivingAddress,
    EditSendingAddress
};

enum EncryptionStatus
{
//    Wallet_Unencrypted = 0,  // !wallet->IsCrypted()
    Wallet_Locked = 0,       // wallet->IsCrypted() && wallet->IsLocked()
    Wallet_Unlocked_MinerOnly,
    Wallet_Unlocked      // wallet->IsCrypted() && !wallet->IsLocked()
};

enum AddressListTableItem
{
    Item_Label = Qt::UserRole + 1,
    Item_Currency,
    Item_Balance,
    Item_Address,
    Item_Frozen
};

enum TransactionsType
{
    Other,
    Generated,
    SendToAddress,
    SendToMining,
    RecvWithAddress,
    RecvFromMining,
    SendToSelf,
    BackDecl,
    TyFailure,
    OpenTicket,
    CloseTicket
};

enum TransactionsListItem
{
    TyItem_Amount = Qt::UserRole + 1,
    TyItem_TypeTy,
    TyItem_StrTypeTy,
    TyItem_TimeData,
    TyItem_FromLabel,
    TyItem_FromAddress,
    TyItem_ToLabel,
    TyItem_ToAddress,
    TyItem_Hash,
    TyItem_Fee,
    TyItem_Execer,
    TyItem_Actionname,
    TyItem_Note,
    TyItem_Error
};

enum TokenTableItem
{
    Item_Account = 0,
    Item_TotalMoney,
    Item_Price,
    Item_Amount,
    Item_Action,
    Item_SellId
};

enum TokenTableItem_2
{
    Item_Token_Addr = Qt::UserRole + 1,
    Item_Token_SellId,
    Item_Token_Price,
    Item_Token_Amount
};

enum SeedTabType
{
    CreateSeed_Tab = 0,
    InputSeed_Tab
};

enum DealModeType {
    SellToken,
    BuyToken
};

enum TransferType {
    ShiftToTrade,
    RollOutTrade
};

enum AssetsTableItem
{
    Item_Symbol = 0,
    Item_AssetsAction,
    Item_Available,
    Item_c2cAction,
    Item_c2cAvailable,
    Item_c2cFreeze
};

enum AssetsTableItem_2
{
    Item_Trade_Symbol = Qt::UserRole + 1,
    Item_Trade_Available,
    Item_Trade_c2cAvailable
};

enum OrderTableItem
{
    Item_Order_Date = 0,
    Item_Order_Symbol,
    Item_Order_BuySell,
    Item_Order_Price,
    Item_Order_Amount,
    Item_Order_SoldBoardlot,
    Item_Order_TxId
};

enum OrderTableItem_2
{
    Item_Order_TxIdKey = Qt::UserRole + 1
};

#endif // ENUMTYPE_H
