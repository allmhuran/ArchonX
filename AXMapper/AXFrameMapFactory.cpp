#undef AX_DEBUGGING

#include "..\AXFrameMap.h"
#include "AXFrameMapFactory.h"
#include "..\AXUtility.h"
#include <iostream>
#include <sstream>
#include <iomanip>

//---------------------------------------------------------------------------------
void AXFrameMapFactory::createDescriptor()
   {
   using namespace std;
   cout << endl
        << "-------------------------------------------------------------" << endl
        << "This wizard will help create a properly structured descriptor" << endl
        << "file."                                                         << endl
        << "It assumes each frame in any one sequence is of constant"      << endl
        << "width and height, and that frames are indexed left to right,"  << endl
        << "top to bottom. If your sequence contains heterogeneous frames" << endl
        << "you can go into the descriptor file afterwards and  edit the " << endl
        << "entries manually."                                             << endl
        << "Also, all frames will be written with an interval of 0! You"   << endl
        << "will probably want to change that."                            << endl        
        << "Sequence lines look like:"                                     << endl
        << ":<name>  <first frame>  <last frame>"                          << endl
        << "Frame lines look like:"                                        << endl
        << "<top>  <left>  <right>  <bottom>  <interval>"                  << endl
        << "-------------------------------------------------------------" << endl << endl;

   system("pause");

   bool usedIndex[AXFrameMap::MAX_SEQUENCES] = { {false} };

   string s;
   unsigned char index;
   unsigned ff, nf, x, y, dx, dy, firstRowPos, framesPerRow;


   cout << endl << "Output filename? ";
   cin >> s;
   fstream out(s.c_str(), ios::out);
   while(true)
      {
      // get a name for the sequence, hash it and make sure the hash isnt already in use
      do {
         cout << endl << "Name of this sequence? (Q = quit): ";
         cin >> s;

         if(s == "Q") 
            return;
         
         index = AXUtility::hash<UC>(s.c_str());
         if(usedIndex[index])
            cout << endl << "That name hashes to an already used index" << endl;
         } while (usedIndex[index]);

      usedIndex[index] = true;
      
      // write out the name
      out << ":" << s;

      // get the first and last frame in this sequence
      cout << endl << "Map index of first frame in sequence (first = 0)? "; 
      cin >> ff;
      out << setw(6) << ff;

      cout << endl << "number of frames in sequence? ";
      cin >> nf;
      out << setw(6) << (ff + nf - 1) << endl;

      cout << endl << "Frames per row on image? ";
      cin >> framesPerRow;

      cout << "Row pos of first frame (first = 1)? ";
      cin >> firstRowPos;

      cout << endl << "Pixel pos of first frame (x y)? ";
      cin >> x >> y;

      cout << endl << "Frame width and height (x y)? ";
      cin >> dx >> dy;      

      cout << endl << "Writing sequence . . .";

      for(int i = firstRowPos; i < (nf + firstRowPos); i++)
         {
         out << setw(6) << x
             << setw(6) << y
             << setw(6) << (x + dx)
             << setw(6) << (y + dy)
             << setw(6) << 0 << endl;
 
         if(i % framesPerRow == 0)
            {
            y += dy;
            x = 0;
            }
         else
            {
            x += dx;
            }
         }

      cout << endl << "Sequence completed." << endl << endl;
      }

   out.close();
   system("pause");
   cout << endl;
   }



//---------------------------------------------------------------------------------
void AXFrameMapFactory::createMap()
   {
   using namespace std;

   std::string descriptor;
   std::string image;
   std::string output;

   cout  << endl << endl << "Descriptor file? ";
   cin >> descriptor;

   cout << endl << "Image file? ";
   cin >> image;

   cout << endl << "Output file? ";
   cin >> output;

   cout << endl << "Creating map . . . " << endl;

   string        line;
   unsigned      nSequences = 0;
   unsigned      nFrames = 0;
   fstream       in(descriptor.c_str(), ios::in);
   fstream       out(output.c_str(), ios::out | ios::binary);

   while(getline(in, line))
      {
      if(line[0] == ':') 
         nSequences++;
      else if(line.length() > 0)
         nFrames++;
      }

   out.write((char*)&nSequences, sizeof nSequences);
   out.write((char*)&nFrames, sizeof nFrames);

   // back to start of file
   in.close();
   in.clear();
   in.open(descriptor.c_str(), ios::in);

   for(int i = 0; i < nSequences; i++)
      writeSequence(in, out);

   in.close();
   in.clear();

   in.open(image.c_str(), ios::in | ios::binary);
   writeImage(in, out);

   in.close();
   out.close();

   cout << "Finished" << endl;
   system("pause");
   cout << endl;
   }

//---------------------------------------------------------------------------------
void AXFrameMapFactory::writeSequence(std::fstream& in, std::fstream& out)
   {
   using namespace std;

   string       line;
   string       sequenceName;
   AXSequence   seq;
   stringstream ss;
   
   getline(in, line);
   if(line == "") 
      return;

   ss << line;
   ss.ignore(1); // eat the colon character
   ss >> sequenceName >> seq._ff >> seq._lf;
   unsigned char index = AXUtility::hash<UC>(sequenceName.c_str());
   out.write((char*)&index, sizeof index);
   out.write((char*)&seq, sizeof(AXSequence));

   for(int i = 0; i <= (seq._lf - seq._ff); i++)
      writeFrame(in, out);   
   }

//---------------------------------------------------------------------------------
void AXFrameMapFactory::writeFrame(std::fstream& in, std::fstream& out)
   {
   using namespace std;
   string line;
   stringstream ss;
   AXFrame frame;

   getline(in, line);
   ss << line;
   ss >> frame._rect.left
      >> frame._rect.top
      >> frame._rect.right
      >> frame._rect.bottom
      >> frame._interval;

   out.write((char*)&frame, sizeof(AXFrame));
   }

//---------------------------------------------------------------------------------
void AXFrameMapFactory::writeImage(std::fstream& in, std::fstream& out)
   {
   using namespace std;

   in.seekg(0, ios::end);
   size_t sz_data = in.tellg();   
   char* imageData = new char[sz_data];
   memset((void*)imageData, 0, sz_data * sizeof(char));
   in.clear();
   in.seekg(0, ios::beg);
   in.read(imageData, sz_data * sizeof(char));

   out.write((char*)&sz_data, sizeof sz_data);
   out.write(imageData, sz_data * sizeof(char));   
   }
