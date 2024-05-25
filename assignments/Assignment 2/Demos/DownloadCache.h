/* Utilities to cache downloaded versions of files from the web. */
#ifndef DownloadCache_Included
#define DownloadCache_Included

#include <fstream>
#include <string>
#include <utility>
#include <memory>
#include <functional>

/* Type representing download status. */
enum class DownloadStatus {
    DOWNLOADING,
    FINISHED
};

/* Callback function used to report progress. */
using DownloadCallback = std::function<void (DownloadStatus)>;

/* Given a URL, returns a stream containing the contents of that URL. This system
 * will attempt to cache things so that they aren't downloaded multiple times.
 *
 * If an error occurs, a DownloadError exception is thrown.
 *
 * You can optionally provide a callback function to receive status updates.
 */
std::shared_ptr<std::istream>
webContentsOf(const std::string& url, const std::string& fileExtension,
              DownloadCallback callback = nullptr);

/* Type representing a downloading error. It just forwards the downloading error along. */
class DownloadError {
public:
    DownloadError(int errorCode) : theErrorCode(errorCode) {

    }

    int errorCode() const {
        return theErrorCode;
    }

private:
    int theErrorCode;
};

#endif
