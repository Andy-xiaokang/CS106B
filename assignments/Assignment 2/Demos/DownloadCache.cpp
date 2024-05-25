#include "DownloadCache.h"
#include "urlstream.h"
using namespace std;

namespace {
    const string kBaseDir = "Downloads/";

    /* Stable hash function to use to compute hash IDs. This hash function is used in
     * place of other hash functions because it computes consistent IDs across program
     * runs.
     *
     * This is the Jenkins 32-bit hash function. It's not the best hash function available,
     * but it's one of the easiest ones to code up. It's adapted from
     * https://en.wikipedia.org/wiki/Jenkins_hash_function.
     */
    uint32_t jenkinsHashOf(const string& str) {
        uint32_t result = 0;
        for (char ch: str) {
            result += static_cast<unsigned char>(ch);
            result += result << 10;
            result ^= result >> 6;
        }
        result += result << 3;
        result ^= result >> 11;
        result += result << 15;
        return result;
    }

    /* Returns the name of the key file and data file to use for a given URL/extension pair,
     * respectively.
     */
    pair<string, string> filenamesFor(const string& url, const string& extension) {
        string hashBase = to_string(jenkinsHashOf(url));

        return make_pair(kBaseDir + hashBase + extension + ".key", kBaseDir + hashBase + extension + ".download");
    }

    /* Returns whether we have a cached version of the given dot string in the indicated
     * base file.
     */
    bool cachedVersionExistsFor(const string& url, const string& extension) {
        auto filenames = filenamesFor(url, extension);
        ifstream keyFile(filenames.first, ios::binary);
        ifstream dataFile(filenames.second, ios::binary);

        /* If the file doesn't exist or can't be opened, then we don't have anything
         * to draw from.
         */
        if (!keyFile || !dataFile) return false;

        /* Make sure that the key file matches the url exactly. */
        ostringstream contents;
        contents << keyFile.rdbuf();
        return contents.str() == url;
    }

    /* We make multiple attempts to download from the server because sometimes the Google servers
     * are flaky and just fail for no reason. :-(
     */
    const size_t kMaxTries = 10;

    /* Downloads the contents of the given URL, returning a string containing
     * the bytes. If the download fails, this throws a DownloadError.
     */
    string download(const string& url) {
        int lastErrorCode = 0;
        for (size_t i = 0; i < kMaxTries; i++) {
            iurlstream downloader;
            downloader.setHeader("User-Agent", "A programming assignment for Stanford's CS106B course. Contact: htiek@cs.stanford.edu");
            downloader.open(url);

            if (!downloader) {
                lastErrorCode = downloader.getErrorCode();
                continue;
            }

            ostringstream builder(ios::binary);
            builder << downloader.rdbuf();
            return builder.str();
        }

        throw DownloadError(lastErrorCode);
    }

    /* Attempts to write the given bytes to disk in the cache. This may fail, in which
     * case we fail silently.
     */
    void cacheBytes(const string& bytes, const string& url, const string& extension) {
        auto filenames = filenamesFor(url, extension);
        ofstream keyFile(filenames.first, ios::binary);
        ofstream dataFile(filenames.second, ios::binary);

        /* These files might not be open. These files might not be writable. But we'll
         * just ignore this.
         */
        keyFile << url;
        dataFile << bytes;
    }
}

shared_ptr<istream>
webContentsOf(const string& url, const string& fileExtension,
              DownloadCallback callback) {
    /* If we already have a cached version of this URL, just return a stream from the
     * cache.
     */
    if (cachedVersionExistsFor(url, fileExtension)) {
        auto filenames = filenamesFor(url, fileExtension);
        return make_shared<ifstream>(filenames.second, ios::binary);
    }

    /* Otherwise, we don't. Go and download it, caching if possible. */
    if (callback) callback(DownloadStatus::DOWNLOADING);
    auto bytes = download(url);
    if (callback) callback(DownloadStatus::FINISHED);

    /* Attempt to write things to disk. This may fail, which we swallow silently. It's
     * annoying, but not world-ending.
     */
    cacheBytes(bytes, url, fileExtension);

    /* Wrap the bytes in an istringstream. */
    return make_shared<istringstream>(bytes, ios::binary);
}
