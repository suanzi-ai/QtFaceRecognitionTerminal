#include <QFile>
#include <fstream>
#include <quface-io/engine.hpp>
#include <quface/logger.hpp>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "config.hpp"
#include "mpi_sys.h"

#include <QDebug>
#include <QImage>
//#include "JQQRCodeReader/JQQRCodeReader.h"
//#include "JQQRCodeWriter/JQQRCodeWriter.h"
#include "zbar.h"

using namespace suanzi;
using namespace suanzi::io;

int main() {
  /*const auto &&qrCodeImage = JQQRCodeWriter::makeQRcode( "this is a test" );
  qDebug() << qrCodeImage;
  if ( qrCodeImage.isNull() )
  {
          return -1;
  }

    qDebug() << "save png succeed:" << qrCodeImage.save("test.png", "PNG");*/

  /*    const QImage testImage( "test.png" );

      JQQRCodeReader qrCodeReader;

      qDebug() << "decodeImage return:" << qrCodeReader.decodeImage( testImage,
     JQQRCodeReader::DecodeQrCodeType );*/

  return 0;
}
