#include "macurlconvert.h"

#import <Foundation/Foundation.h>

QUrl MacUrlConvert::convertUrl(const QUrl &url) {
        NSString *string = url.toString().toNSString();
        NSString *string2 = [[[NSURL URLWithString: string] filePathURL] absoluteString];
        QUrl retval = QUrl(QString::fromNSString(string2));
        return retval;
}
