/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "apeHttpManager.h"
#include <sstream>
#include <iostream>
#ifdef HTTPMANAGER_USE_CURL
	#include <curl/curl.h>
	#include <curl/easy.h>
#endif

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string data((const char*) ptr, (size_t) size * nmemb);
    *((std::stringstream*) stream) << data << std::endl;
    return size * nmemb;
}

size_t write_file(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

ape::HttpManager::HttpManager()
{
#ifdef HTTPMANAGER_USE_CURL
    mpCurl = curl_easy_init();
#endif
}

ape::HttpManager::~HttpManager()
{
#ifdef HTTPMANAGER_USE_CURL
    curl_easy_cleanup(mpCurl);
#endif
}

bool ape::HttpManager::downloadResources(const std::string& url, const std::string& location, const std::string& md5)
{
#ifdef HTTPMANAGER_USE_CURL
	if (mpCurl)
	{
		FILE *fp;
		auto posLastFwdSlash = url.find_last_of("/");
		//auto posLastDot = url.find_last_of(".");
		auto fileName = url.substr(posLastFwdSlash + 1, url.length());
		std::stringstream filePath;
		std::size_t found = location.find(":");
		if (found != std::string::npos)
		{
			filePath << location << "/" << fileName;
		}
		found = location.find("./");
		if (found != std::string::npos)
		{
			filePath << location << "/" << fileName;
		}
		else
		{
			std::stringstream resourceLocationPath;
			resourceLocationPath << APE_SOURCE_DIR << location;
			filePath << resourceLocationPath.str() << "/" << fileName;
		}
		APE_LOG_DEBUG("try to download from: " << url << " to: " << filePath.str());
		fp = fopen(filePath.str().c_str(), "wb");
		curl_easy_setopt(mpCurl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(mpCurl, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(mpCurl, CURLOPT_WRITEDATA, fp);
		//if (md5.size())
		//{
		//	curl_easy_setopt(mpCurl, CURLOPT_URL, md5.c_str());
		//	//curl_easy_setopt(mpCurl, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, md5);
		//}
		CURLcode res = curl_easy_perform(mpCurl);
		if (res != CURLE_OK)
		{
			APE_LOG_DEBUG("curl_easy_perform() failed: " << curl_easy_strerror(res));
		}
		fclose(fp);
	}
#endif
	return false;
}

std::string ape::HttpManager::download(const std::string& url)
{
	std::stringstream out;
#ifdef HTTPMANAGER_USE_CURL
    curl_easy_setopt(mpCurl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(mpCurl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(mpCurl, CURLOPT_NOSIGNAL, 1); 
    curl_easy_setopt(mpCurl, CURLOPT_ACCEPT_ENCODING, "deflate");
    
    curl_easy_setopt(mpCurl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(mpCurl, CURLOPT_WRITEDATA, &out);

    CURLcode res = curl_easy_perform(mpCurl);

    if (res != CURLE_OK)
	{
        APE_LOG_ERROR("curl_easy_perform() failed: " << curl_easy_strerror(res));
	}
#endif
    return out.str();
}

std::string ape::HttpManager::post(const std::string& url, const std::string& data)
{
	std::stringstream out;
#ifdef HTTPMANAGER_USE_CURL
	struct curl_slist *headers;

	headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(mpCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(mpCurl, CURLOPT_FOLLOWLOCATION, 1L);
	
	curl_easy_setopt(mpCurl, CURLOPT_NOSIGNAL, 1); 
	curl_easy_setopt(mpCurl, CURLOPT_ACCEPT_ENCODING, "deflate");
	
	curl_easy_setopt(mpCurl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(mpCurl, CURLOPT_WRITEDATA, &out);

	curl_easy_setopt(mpCurl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(mpCurl, CURLOPT_POST, 1L);

	curl_easy_setopt(mpCurl, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(mpCurl, CURLOPT_POSTFIELDSIZE, data.length());

	CURLcode res = curl_easy_perform(mpCurl);

	if (res != CURLE_OK) 
	{
		APE_LOG_ERROR("curl_easy_perform() failed: " << curl_easy_strerror(res));
	}
#endif
	return out.str();
}

std::string ape::HttpManager::del(const std::string& url, const std::string& data)
{
	std::stringstream out;
#ifdef HTTPMANAGER_USE_CURL
	struct curl_slist *headers;

	headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(mpCurl, CURLOPT_CUSTOMREQUEST, "DELETE");

	curl_easy_setopt(mpCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(mpCurl, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(mpCurl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(mpCurl, CURLOPT_ACCEPT_ENCODING, "deflate");

	curl_easy_setopt(mpCurl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(mpCurl, CURLOPT_WRITEDATA, &out);

	curl_easy_setopt(mpCurl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(mpCurl, CURLOPT_POST, 1L);

	curl_easy_setopt(mpCurl, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(mpCurl, CURLOPT_POSTFIELDSIZE, data.length());

	CURLcode res = curl_easy_perform(mpCurl);

	if (res != CURLE_OK)
	{
		APE_LOG_ERROR("curl_easy_perform() failed: " << curl_easy_strerror(res));
	}
#endif
	return out.str();
}
