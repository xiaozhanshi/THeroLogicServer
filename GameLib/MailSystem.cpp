#include "DBOperator.h"
#include "PlatFunc.h"
#include "MailSystem.h"
#include "log.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "qredis.h"

using namespace std;

/* This is a simple example showing how to send mail using libcurl's SMTP
 * capabilities. It builds on the simplesmtp.c example, adding some
 * authentication and transport security.
 */

#define FROM    "<jacketzhong@gmail.com>"
#define TO      "<xroom007@gmail.com>"
#define CC      "<winktzhong@gmail.com>"

// cccc1
#define FROMQQ    "<2354558781@qq.com>"
#define TOQQ      "<2354558781@qq.com>"
#define CCQQ      "<2354558781@qq.com>"

// cccc1
#define FROMHOT    "<winktzhong@hotmail.com>"
#define TOHOT      "<winktzhong@hotmail.com>"
#define CCHOT      "<winktzhong@hotmail.com>"

#define FROMYAHOO    "<winktzhong@yahoo.cn>"
#define TOYAHOO      "<winktzhong@yahoo.cn>"
#define CCYAHOO      "<winktzhong@yahoo.cn>"

static const char *payload_text[]={
    "Date: Mon, 28 Nov 2011 21:54:29 +1100\n",
    "To: " TO "\n",
    "From: " FROM "(micogamobile)\n",
    "Cc: " CC "(Another example User)\n",
    "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@rfcpedant.example.org>\n",
    "Subject: micogamobile password message\n",
    "\n", /* empty line to divide headers from body, see RFC5322 */
    "from micogamobile.\n",
    "\n",
    "thank you for playing my game.\n",
    "Check RFC5322.\n",
    NULL
};

struct upload_status {
    int lines_read;
};

CMailSystem CMailSystem::sInstance;

CMailSystem& CMailSystem::instance()
{
	return sInstance;
}

CMailSystem::CMailSystem()
{

}

CMailSystem::~CMailSystem()
{

}

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data;
    
    if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
        return 0;
    }
    
    data = payload_text[upload_ctx->lines_read];
    
    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read ++;
        return len;
    }
    return 0;
}

int sent_To_gmail(string & mailaddress, string & password)
{
    char *format[] = {"%a, %d %b %y %H:%M:%S %Z", NULL};
    char buf[30];
    char buf2[50];
	char mail[50];
	char pass[2000] = {0};

	//pass = (char*)malloc(sizeof(char)*(password.size() + 10));
    int i;
    time_t clock;
    struct tm *tm;
    time(&clock);
    tm = localtime(&clock);
    for(i = 0; format[i] != NULL; i++)
    {
        strftime(buf, sizeof(buf), format[i], tm);
    }
	log_debug("to format is %s\n", payload_text[1]);
    sprintf(buf2, "Date: %s\n", buf);
	sprintf(mail, "To: <%s> \n", mailaddress.c_str());//"xroom007@gmail.com");//mailaddress.c_str());
    log_debug("the date is %s mail is %s\n", buf2, mail);
	sprintf(pass, "you password is %s\n", password.c_str());

    payload_text[0] = buf2;
    payload_text[1] = mail;
	payload_text[10] = pass;
	//sprintf(mail, "Cc:  <%s> \n", mailaddress.c_str());
	//payload_text[3] = mail;

    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;
    
    upload_ctx.lines_read = 0;
    
    curl = curl_easy_init();
    if (curl) {
        /* This is the URL for your mailserver. Note the use of port 587 here,
         * instead of the normal SMTP port (25). Port 587 is commonly used for
         * secure mail submission (see RFC4403), but you should use whatever
         * matches your server configuration. */
        
        //cccccc2
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        //curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.live.com:587");
        /* In this example, we'll start with a plain text connection, and upgrade
         * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
         * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
         * will continue anyway - see the security discussion in the libcurl
         * tutorial for more details. */
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        
        /* If your server doesn't have a valid certificate, then you can disable
         * part of the Transport Layer Security protection by setting the
         * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
         * That is, in general, a bad idea. It is still better than sending your
         * authentication details in plain text though.
         * Instead, you should get the issuer certificate (or the host certificate
         * if the certificate is self-signed) and add it to the set of certificates
         * that are known to libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See
         * docs/SSLCERTS for more information.
         */
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");
        
        /* A common reason for requiring transport security is to protect
         * authentication details (user names and passwords) from being "snooped"
         * on the network. Here is how the user name and password are provided: */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "jacketzhong@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "zqq5201314king");
		
		//curl_easy_setopt(curl, CURLOPT_USERNAME, "winktzhong@hotmail.com");
		//curl_easy_setopt(curl, CURLOPT_PASSWORD, "zqq5201314king");
        //ccccc3
        //        curl_easy_setopt(curl, CURLOPT_USERNAME, "495347579@qq.com");
        //        curl_easy_setopt(curl, CURLOPT_PASSWORD, "520131419860302");
        
        /* value for envelope reverse-path */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);
        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, TO);
        recipients = curl_slist_append(recipients, CC);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        /* In this case, we're using a callback function to specify the data. You
         * could just use the CURLOPT_READDATA option to specify a FILE pointer to
         * read from.
         */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        
        /* Since the traffic will be encrypted, it is very useful to turn on debug
         * information within libcurl to see what is happening during the transfer.
         */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        /* send the message (including headers) */
        res = curl_easy_perform(curl);

        /* Check for errors */
		char errstring [300] = {'o', 'k', 0};
		int ret = 0;
        if(res != CURLE_OK)
		{
            sprintf(errstring, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
			ret = 2;
		}
		
		log_debug("the err is %s\n", errstring);
        
        /* free the list of recipients and clean up */
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);

		//if (pass != NULL)
		//{
		//	free(pass);
		//	pass = NULL;
		//}
		return ret;
    }
	//if (pass != NULL)
	//{
	//	free(pass);
	//	pass = NULL;
	//}
	return 1;
}

void sent_To_qq()
{
    char *format[] = {"%a, %d %b %y %H:%M:%S %Z", NULL};
    char buf[30];
    char buf2[50];
    int i;
    time_t clock;
    struct tm *tm;
    time(&clock);
    tm = localtime(&clock);
    for(i = 0; format[i] != NULL; i++)
    {
        strftime(buf, sizeof(buf), format[i], tm);
    }
    sprintf(buf2, "Date: %s\n", buf);
    log_debug("the date is %s\n", buf2);
    payload_text[0] = buf2;
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;
    
    upload_ctx.lines_read = 0;
    
    curl = curl_easy_init();
    if (curl) {
        /* This is the URL for your mailserver. Note the use of port 587 here,
         * instead of the normal SMTP port (25). Port 587 is commonly used for
         * secure mail submission (see RFC4403), but you should use whatever
         * matches your server configuration. */
        
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.qq.com:25");
        
        /* In this example, we'll start with a plain text connection, and upgrade
         * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
         * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
         * will continue anyway - see the security discussion in the libcurl
         * tutorial for more details. */
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        //curl_easy_setopt(curl, CURLOPT_FTPSSLAUTH, CURLFTPAUTH_SSL);
        /* If your server doesn't have a valid certificate, then you can disable
         * part of the Transport Layer Security protection by setting the
         * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
         * That is, in general, a bad idea. It is still better than sending your
         * authentication details in plain text though.
         * Instead, you should get the issuer certificate (or the host certificate
         * if the certificate is self-signed) and add it to the set of certificates
         * that are known to libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See
         * docs/SSLCERTS for more information.
         */
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");
        
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        
        /* A common reason for requiring transport security is to protect
         * authentication details (user names and passwords) from being "snooped"
         * on the network. Here is how the user name and password are provided: */
        
        //ccccc3
        curl_easy_setopt(curl, CURLOPT_USERNAME, "2354558781@qq.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "zh5201314li");
        
        /* value for envelope reverse-path */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROMQQ);
        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, TOQQ);
        recipients = curl_slist_append(recipients, CCQQ);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        /* In this case, we're using a callback function to specify the data. You
         * could just use the CURLOPT_READDATA option to specify a FILE pointer to
         * read from.
         */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        
        /* Since the traffic will be encrypted, it is very useful to turn on debug
         * information within libcurl to see what is happening during the transfer.
         */
        /* send the message (including headers) */
        res = curl_easy_perform(curl);
        
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* free the list of recipients and clean up */
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

void sent_To_hotmail()
{
    char *format[] = {"%a, %d %b %y %H:%M:%S %Z", NULL};
    char buf[30];
    char buf2[50];
    int i;
    time_t clock;
    struct tm *tm;
    time(&clock);
    tm = localtime(&clock);
    for(i = 0; format[i] != NULL; i++)
    {
        strftime(buf, sizeof(buf), format[i], tm);
    }
    sprintf(buf2, "Date: %s\n", buf);
    log_debug("the date is %s\n", buf2);
    payload_text[0] = buf2;
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;
    
    upload_ctx.lines_read = 0;
    
    curl = curl_easy_init();
    if (curl) {
        /* This is the URL for your mailserver. Note the use of port 587 here,
         * instead of the normal SMTP port (25). Port 587 is commonly used for
         * secure mail submission (see RFC4403), but you should use whatever
         * matches your server configuration. */
        
        //cccccc2        
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.live.com:587");
        /* In this example, we'll start with a plain text connection, and upgrade
         * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
         * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
         * will continue anyway - see the security discussion in the libcurl
         * tutorial for more details. */
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        
        /* If your server doesn't have a valid certificate, then you can disable
         * part of the Transport Layer Security protection by setting the
         * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
         * That is, in general, a bad idea. It is still better than sending your
         * authentication details in plain text though.
         * Instead, you should get the issuer certificate (or the host certificate
         * if the certificate is self-signed) and add it to the set of certificates
         * that are known to libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See
         * docs/SSLCERTS for more information.
         */
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

        /* A common reason for requiring transport security is to protect
         * authentication details (user names and passwords) from being "snooped"
         * on the network. Here is how the user name and password are provided: */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "winktzhong@hotmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "zqq5201314king");

        //ccccc3
        //        curl_easy_setopt(curl, CURLOPT_USERNAME, "495347579@qq.com");
        //        curl_easy_setopt(curl, CURLOPT_PASSWORD, "520131419860302");
        
        /* value for envelope reverse-path */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROMHOT);
        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, TOHOT);
        recipients = curl_slist_append(recipients, CCHOT);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        /* In this case, we're using a callback function to specify the data. You
         * could just use the CURLOPT_READDATA option to specify a FILE pointer to
         * read from.
         */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        
        /* Since the traffic will be encrypted, it is very useful to turn on debug
         * information within libcurl to see what is happening during the transfer.
         */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        /* send the message (including headers) */
        res = curl_easy_perform(curl);
        
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* free the list of recipients and clean up */
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

void sent_To_yahoo()
{
    char *format[] = {"%a, %d %b %y %H:%M:%S %Z", NULL};
    char buf[30];
    char buf2[50];
    int i;
    time_t clock;
    struct tm *tm;
    time(&clock);
    tm = localtime(&clock);
    for(i = 0; format[i] != NULL; i++)
    {
        strftime(buf, sizeof(buf), format[i], tm);
    }
    sprintf(buf2, "Date: %s\n", buf);
    log_debug("the date is %s\n", buf2);
    payload_text[0] = buf2;
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;
    
    upload_ctx.lines_read = 0;
    
    curl_global_init(CURL_GLOBAL_SSL);
    curl = curl_easy_init();
    
    if (curl) {
        /* This is the URL for your mailserver. Note the use of port 587 here,
         * instead of the normal SMTP port (25). Port 587 is commonly used for
         * secure mail submission (see RFC4403), but you should use whatever
         * matches your server configuration. */
        
        //cccccc2        
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.mail.yahoo.cn:465");
        /* In this example, we'll start with a plain text connection, and upgrade
         * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
         * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
         * will continue anyway - see the security discussion in the libcurl
         * tutorial for more details. */
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        

        /* If your server doesn't have a valid certificate, then you can disable
         * part of the Transport Layer Security protection by setting the
         * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
         *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
         * That is, in general, a bad idea. It is still better than sending your
         * authentication details in plain text though.
         * Instead, you should get the issuer certificate (or the host certificate
         * if the certificate is self-signed) and add it to the set of certificates
         * that are known to libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See
         * docs/SSLCERTS for more information.
         */
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        //curl_easy_setopt(curl, CURLOPT_CAPATH, "./");
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* A common reason for requiring transport security is to protect
         * authentication details (user names and passwords) from being "snooped"
         * on the network. Here is how the user name and password are provided: */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "winktzhong@yahoo.cn");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "zqq5201314king");
        //ccccc3
        //        curl_easy_setopt(curl, CURLOPT_USERNAME, "495347579@qq.com");
        //        curl_easy_setopt(curl, CURLOPT_PASSWORD, "520131419860302");
        
        /* value for envelope reverse-path */
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROMYAHOO);
        /* Add two recipients, in this particular case they correspond to the
         * To: and Cc: addressees in the header, but they could be any kind of
         * recipient. */
        recipients = curl_slist_append(recipients, TOYAHOO);
        recipients = curl_slist_append(recipients, CCYAHOO);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        
        /* In this case, we're using a callback function to specify the data. You
         * could just use the CURLOPT_READDATA option to specify a FILE pointer to
         * read from.
         */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        
        /* Since the traffic will be encrypted, it is very useful to turn on debug
         * information within libcurl to see what is happening during the transfer.
         */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        /* send the message (including headers) */
        res = curl_easy_perform(curl);
        
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* free the list of recipients and clean up */
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

int CMailSystem::SendEmail(string & mailaddress, string & password)
{
	int ret = sent_To_gmail(mailaddress, password);
	return ret;
}

FILE * fp;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE*)fp);
	printf("the nmemb is %d, the size is %d\n", nmemb, size);
	return written;
}

void CMailSystem::GetIPandSent()
{
	CURL *curl;

	curl_global_init(CURL_GLOBAL_ALL);
	curl=curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.net.cn/static/customercare/yourIP.asp");

	if((fp=fopen("getip.txt","w"))==NULL)
	{
		curl_easy_cleanup(curl);
		return;
	}
	////CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(fp);
	ifstream out;
	string str = "getip.txt";
	out.open(str.c_str(), ios::in);
	string line;
	string password;
	while(!out.eof()){
		std::getline(out,line);
		size_t found;
		found=line.find("<CENTER>");
		if (found!=string::npos)
		{
			std::getline(out,line);
			password += line;
		}
		size_t found2;
		found2=line.find("</CENTER>");
		if (found!=string::npos)
		{
			break;
		}
	}
	out.close();

	// 过滤ip
	if (password.size() >= 9)
	{
		int startindex = 0;
		int endindex = 8;
		int len = 0;
		size_t found;
		found=line.find("<h2>");
		if (found!=string::npos)
		{
			startindex = found + 4;
		}

		size_t found2;
		found2=line.find("</h2>");
		if (found2!=string::npos)
		{
			endindex = found2;
		}

		len = endindex - startindex;
		if (len > 4)
		{
			string substring;
			substring = password.substr(startindex, len);
			password = substring;

			// set redis
			if (g_qredis.CheckIsConnect())
			{
				// key
				char key[100] = {0};
				sprintf(key, "WANIP"); 
				// value
				char valueip[200] = {0};
				sprintf(valueip, "%s", password.c_str());
				g_qredis.Set(key, valueip);
			}
		}
	}

	log_debug("the ip is %s\n, len is %d\n", password.c_str(), password.size());
	string mailaddress = "xroom007@gmail.com";
	sent_To_gmail(mailaddress, password);
}
