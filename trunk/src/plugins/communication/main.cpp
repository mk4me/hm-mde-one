/**
@author Marek Daniluk
*/
#include <QtGui/QApplication>
#include "core/CommunicationService.h"
#include "CommServ.h"

using namespace EDRCM;

std::string parse(char* argument) {
	std::string temp(argument);
	temp = temp.substr(2, temp.length());
	return temp;
}

int main(int argc, char* argv[]) {
	//std::string ftps_addr = FTPS_cURL::ftps_addr, ftps_usr = FTPS_cURL::ftps_usr, ftps_pswd = FTPS_cURL::ftps_pswd;
	//std::string wsdl_usr = WSDL_Wsdlpull::wsdl_usr, wsdl_pswd = WSDL_Wsdlpull::wsdl_pswd;
	//std::string filestoreman_uri = FileStoremanService::default_uri, basicqueries_uri = BasicQueriesService::default_uri, basicupdates_uri = BasicUpdatesService::default_uri;
	//if(argc > 1) {
	//	for(int i = 1; i < argc; i++) {
	//		std::string temp = parse(argv[i]);
	//		switch(argv[i][0])
	//		{
	//		case 'h':
	//			{
	//				std::cout << "Options:\n\th help\n";
	//				std::cout << "\tfa[uri] FTP address URI\n\tfu[user] FTP user\n\tfp[password] FTP user's password\n";
	//				std::cout << "\twu[user] WSDL user\n\twp[password] WSDL user's password\n";
	//				std::cout << "\twf[uri] URI to FileStoremanService document\n\twb[uri] URI to BasicUpdatesService document\n\twq[uri] URI to BasicQueriesService document\n";
	//				return 0;
	//			}
	//		case 'f':
	//			{
	//				switch(argv[i][1])
	//				{
	//				case 'a':
	//					{
	//						ftps_addr = temp;
	//						break;
	//					}
	//				case 'u':
	//					{
	//						ftps_usr = temp;
	//						break;
	//					}
	//				case 'p':
	//					{
	//						ftps_pswd = temp;
	//						break;
	//					}
	//				default:
	//					{
	//						std::cout << "There is no operation like " << argv[i][0] << argv[i][1] << " Use h to see help.\n";
	//						break;
	//					}
	//				}
	//				break;
	//			}
	//		case 'w':
	//			{
	//				switch(argv[i][1])
	//				{
	//				case 'u':
	//					{
	//						wsdl_usr = temp;
	//						break;
	//					}
	//				case 'p':
	//					{
	//						wsdl_pswd = temp;
	//						break;
	//					}
	//				case 'f':
	//					{
	//						filestoreman_uri = temp;
	//						break;
	//					}
	//				case 'q':
	//					{
	//						basicqueries_uri = temp;
	//						break;
	//					}
	//				case 'b':
	//					{
	//						basicupdates_uri = temp;
	//						break;
	//					}
	//				default:
	//					{
	//						std::cout << "There is no operation like " << argv[i][0] << argv[i][1] << " Use h to see help.\n";
	//						break;
	//					}
	//				}
	//				break;
	//			}
	//		default:
	//			{
	//				break;
	//			}
	//		}
	//	}
	//}
	//CommunicationService* db;
	//TransportWSDL_FTPS* t = new TransportWSDL_FTPS();
	//QueryWSDL* q = new QueryWSDL();

	//t->setFTPCredentials(ftps_addr, ftps_usr, ftps_pswd);
	//t->setWSCredentials(filestoreman_uri, wsdl_usr, wsdl_pswd);

	//q->setWSCredentials(WSDL_Wsdlpull::wsdl_usr, WSDL_Wsdlpull::wsdl_pswd);
	//q->setBasicQueriesServiceUri(basicqueries_uri);
	//q->setBasicUpdatesServiceUri(basicupdates_uri);
	//try {
	//	db = CommunicationService::createInstanceFirstTime();
	//	db->setTransportManager(t);
	//	db->setQueryManager(q);
	//	//std::cout << db->storeSessionFile(1, "directory", "dziennik logow", "log.txt") << "\n";
	//	std::cout << db->storeTrialFile(2, "directory", "dziennik logow dla performera", "log.txt") << "\n";
	//} catch(EDRException& e) {
	//	std::cout << e.getExceptionMessage() << "\n";
	//	EDRException::log(e.getExceptionMessage());
	//}
	//db->destoryInstance();
	//delete t;
	//delete q;

	//FileStoremanService fss;
	//fss.setCredentials(FileStoremanService::default_uri, WSDL_Wsdlpull::wsdl_usr, WSDL_Wsdlpull::wsdl_pswd);
	//try {
	//	std::cout << ftp << std::endl;
	//} catch(EDRException& e) {
	//	std::cout << e.getExceptionMessage() << std::endl;
	//	e.log("URI: " + FileStoremanService::default_uri);
	//	e.log("User: " + fss.getUser());
	//	e.log("Pass: " + fss.getPassword());
	//	e.log("Exception: " + e.getExceptionMessage());
	//}
	QApplication a(argc, argv);
	CommServ commserv;
	commserv.show();
	return commserv.exec();
}
