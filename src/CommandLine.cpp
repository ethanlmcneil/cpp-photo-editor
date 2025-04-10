/**
 * @file CommandLine.cpp
 * @brief Command line parsing logic for either image or volume mode,
 *        with optional filtering/processing operations.
 * 
 * Group Members:
 * - Jiaqi    (GitHub: esemsc-jc1424)
 * - Daicong  (GitHub: esemsc-c730ef50)
 * - Ida      (GitHub: esemsc-ifc24)
 * - Zhuyi    (GitHub: esemsc-zf1124)
 * - Dany     (GitHub: esemsc-dh324)
 * - Ethan    (GitHub: edsml-elm224)
 * - Keyun    (GitHub: esemsc-km824)
 */

 #include "CommandLine.h"
 #include <iostream>
 #include <cstdlib>
 #include <cctype>
 #include <algorithm>
 
 /**
  * @brief Removes all leading and trailing whitespace characters
  *        (spaces or tabs) from the provided string.
  *
  * @param s The string to be trimmed in-place.
  */
 static void trim(std::string &s) {
     // Remove leading whitespace
     while(!s.empty() && (s.front()==' ' || s.front()=='\t')) {
         s.erase(s.begin());
     }
     // Remove trailing whitespace
     while(!s.empty() && (s.back()==' ' || s.back()=='\t')) {
         s.pop_back();
     }
 }
 
 /**
  * @brief Checks if a string represents a numeric value
  *        (possibly with a minus sign or decimal point).
  *
  * @param s The string to check.
  * @return true if the string contains at least one digit and only
  *         digits, a dot, or a minus sign.
  */
 static bool isNumeric(const std::string &s){
     if(s.empty()) return false;
     bool hasDigit=false;
     for(char c: s){
         // Acceptable characters: digits, dot, minus sign
         if((c>='0'&& c<='9')|| c=='.'|| c=='-'){
             // If we see any digit, set hasDigit to true
             if(c>='0'&& c<='9') hasDigit=true;
         } else {
             // Any other character invalidates the numeric check
             return false;
         }
     }
     // Must have encountered at least one digit
     return hasDigit;
 }
 
 /**
  * @brief Attempts to split tokens that appear combined, like "2.0-s"
  *        into separate tokens: ["2.0","-s"]. Also handles strings such
  *        as "ThinSlabSliceXZ" to yield tokens like ["-s","XZ"].
  *
  * @param input The original token that might contain embedded markers.
  * @return A vector of extracted tokens after special-case splitting.
  */
 static std::vector<std::string> splitCompatibility(const std::string &input){
     std::vector<std::string> out;
 
     // 1) Detect a pattern such as "2.0-s" by finding a dash with
     //    numeric content to the left and a known short flag to the right.
     size_t dashPos= input.find('-');
     if(dashPos!=std::string::npos && dashPos>0 && dashPos< input.size()-1){
         std::string left= input.substr(0,dashPos);
         std::string right=input.substr(dashPos);  // e.g. "-s"
         // If the left side is numeric and right side is one of these short flags:
         if(isNumeric(left) && (right=="-s"||right=="-p"||right=="-l"||right=="-f")){
             out.push_back(left);
             out.push_back(right);
             return out; // Return immediately after splitting
         }
     }
 
     // 2) If the input begins with "ThinSlabSlice"
     if(input.rfind("ThinSlabSlice",0)==0){
         // Remove "ThinSlabSlice" prefix
         std::string remainder= input.substr( std::string("ThinSlabSlice").size() );
         // Add "-s" as the recognized slice flag
         out.push_back("-s");
 
         // Check if remainder starts with "XZ", "YZ", or "XY"
         if(remainder.rfind("XZ",0)==0){
             out.push_back("XZ");
             remainder= remainder.substr(2);
         } else if(remainder.rfind("YZ",0)==0){
             out.push_back("YZ");
             remainder= remainder.substr(2);
         } else if(remainder.rfind("XY",0)==0){
             out.push_back("XY");
             remainder= remainder.substr(2);
         }
 
         // If there's still leftover text, store it as an additional token
         if(!remainder.empty()){
             out.push_back(remainder);
         }
         return out;
     }
 
     // If the input begins with "ThinSlabProjection"
     if(input.rfind("ThinSlabProjection",0)==0){
         // Remove "ThinSlabProjection" prefix
         std::string remainder= input.substr(std::string("ThinSlabProjection").size());
         // Add "-p" as the recognized projection flag
         out.push_back("-p");
         if(!remainder.empty()){
             out.push_back(remainder);
         }
         return out;
     }
 
     // If nothing matched the above rules, just return the input as is
     out.push_back(input);
     return out;
 }
 
 /**
  * @brief Parses command-line arguments, expecting either image or volume mode,
  *        then collecting any filter or volume operations in sequence.
  *
  * @param argc Argument count (from main).
  * @param argv Argument vector (from main).
  * @return A CommandOptions struct with the results of parsing.
  */
 CommandOptions CommandLine::parseArgs(int argc, char** argv)
 {
     CommandOptions opts;
 
     // Require at least 3 arguments: e.g., ./Program -i input.png output.png
     if(argc<3){
         std::cerr<<"ERROR: Not enough arguments!\n";
         std::exit(1);
     }
 
     // First argument => decide image or volume
     std::string firstArg= argv[1];
     if(firstArg=="-i"){
         opts.isImage= true;
     } else if(firstArg=="-d"){
         opts.isVolume= true;
     } else {
         std::cerr<<"ERROR: First option must be -i <image> or -d <volume>.\n";
         std::exit(1);
     }
 
     // Second argument => input path
     opts.inputPath= argv[2];
 
     // Last argument => output path
     opts.outputPath= argv[argc-1];
 
     // Collect middle arguments, splitting them if needed
     std::vector<std::string> tokens;
     for(int i=3; i< (argc-1); i++){
         std::string raw= argv[i];
         trim(raw);
         if(raw.empty()) continue;
 
         // Break up cases like "2.0-s" or "ThinSlabSliceXZ" as separate tokens
         auto splitted= splitCompatibility(raw);
         for(auto &x: splitted){
             if(!x.empty()) tokens.push_back(x);
         }
     }
 
     // Now parse the tokens in order
     for(size_t i=0; i< tokens.size(); i++){
         std::string t= tokens[i];
 
         // If it's volume mode, check for first/last index or extension
         if(opts.isVolume && (t=="-f"||t=="--first")){
             // Must have another token for the index
             if(i+1>= tokens.size()){
                 std::cerr<<"ERROR: "<< t <<" requires <index>\n";
                 std::exit(1);
             }
             i++;
             opts.firstIndex= std::atoi(tokens[i].c_str());
             continue;
         }
         if(opts.isVolume && (t=="-l"||t=="--last")){
             if(i+1>= tokens.size()){
                 std::cerr<<"ERROR: "<< t <<" requires <index>\n";
                 std::exit(1);
             }
             i++;
             opts.lastIndex= std::atoi(tokens[i].c_str());
             continue;
         }
         if(opts.isVolume && (t=="-x"||t=="--extension")){
             if(i+1>= tokens.size()){
                 std::cerr<<"ERROR: "<< t <<" requires <ext>\n";
                 std::exit(1);
             }
             i++;
             opts.volumeExt= tokens[i];
             continue;
         }
 
         // Create a FilterOption object for storing operation details
         FilterOption fo;
 
         // If in image mode, interpret tokens as 2D filter or operation
         if(opts.isImage){
             if(t=="-g"||t=="--greyscale"){
                 fo.name="greyscale"; 
                 opts.operations.push_back(fo);
             }
             else if(t=="-b"||t=="--brightness"){
                 if(i+1>= tokens.size()){
                     std::cerr<<"ERROR: brightness requires <value>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="brightness";
                 fo.floats.push_back(std::atof(tokens[i].c_str()));
                 opts.operations.push_back(fo);
             }
             else if(t=="-h"||t=="--histogram"){
                 if(i+1>= tokens.size()){
                     std::cerr<<"ERROR: histogram requires <type>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="histogram";
                 fo.subtype= tokens[i];
                 opts.operations.push_back(fo);
             }
             else if(t=="-r"||t=="--blur"){
                 // For blur => next token is <type>, then <size>, then optional stdev
                 if(i+2>= tokens.size()){
                     std::cerr<<"ERROR: blur requires <type> <size>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="blur";
                 fo.subtype= tokens[i];
                 i++;
                 float sizeVal= std::atof(tokens[i].c_str());
                 fo.floats.push_back(sizeVal);
 
                 // If next token is numeric, treat it as stdev
                 if(i+1< tokens.size() && isNumeric(tokens[i+1])){
                     i++;
                     float stdev= std::atof(tokens[i].c_str());
                     fo.floats.push_back(stdev);
                 }
                 opts.operations.push_back(fo);
             }
             else if(t=="-e"||t=="--edge"){
                 if(i+1>= tokens.size()){
                     std::cerr<<"ERROR: edge requires <type>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="edge";
                 fo.subtype= tokens[i];
                 opts.operations.push_back(fo);
             }
             else if(t=="-p"||t=="--sharpen"){
                 fo.name="sharpen"; 
                 opts.operations.push_back(fo);
             }
             else if(t=="-n"||t=="--saltpepper"){
                 if(i+1>= tokens.size()){
                     std::cerr<<"ERROR: saltpepper requires <amount>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="saltpepper";
                 fo.floats.push_back(std::atof(tokens[i].c_str()));
                 opts.operations.push_back(fo);
             }
             else if(t=="-t"||t=="--threshold"){
                 if(i+2>= tokens.size()){
                     std::cerr<<"ERROR: threshold requires <value> <type>\n";
                     std::exit(1);
                 }
                 i++;
                 float val= std::atof(tokens[i].c_str());
                 i++;
                 fo.name="threshold";
                 fo.floats.push_back(val);
                 fo.subtype= tokens[i];
                 opts.operations.push_back(fo);
             }
             else {
                 // Unrecognized token in image mode
                 std::cerr<<"WARNING: Unrecognized token \""<< t <<"\" (image mode). Will ignore.\n";
             }
         }
 
         // If in volume mode, interpret tokens as 3D operations
         else if(opts.isVolume){
             if(t=="-r"||t=="--blur"){
                 if(i+2>= tokens.size()){
                     std::cerr<<"ERROR: blur requires <type> <size>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="blur";
                 fo.subtype= tokens[i];
                 i++;
                 float sizeVal= std::atof(tokens[i].c_str());
                 fo.floats.push_back(sizeVal);
 
                 // If next token is numeric, treat it as stdev
                 if(i+1< tokens.size() && isNumeric(tokens[i+1])){
                     i++;
                     float stdev= std::atof(tokens[i].c_str());
                     fo.floats.push_back(stdev);
                 }
                 opts.operations.push_back(fo);
             }
             else if(t=="-s"||t=="--slice"){
                 if(i+2>= tokens.size()){
                     std::cerr<<"ERROR: slice requires <plane> <const>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="slice";
                 fo.subtype= tokens[i];
                 i++;
                 fo.floats.push_back(std::atof(tokens[i].c_str()));
                 opts.operations.push_back(fo);
             }
             else if(t=="-p"||t=="--projection"){
                 if(i+1>= tokens.size()){
                     std::cerr<<"ERROR: projection requires <type>\n";
                     std::exit(1);
                 }
                 i++;
                 fo.name="projection";
                 fo.subtype= tokens[i];
                 opts.operations.push_back(fo);
             }
             else {
                 // Unrecognized token in volume mode
                 std::cerr<<"WARNING: Unrecognized token \""<< t <<"\" (volume mode). Will ignore.\n";
             }
         }
     }
 
     // Require at least one of isImage or isVolume
     if(!opts.isImage && !opts.isVolume){
         std::cerr<<"ERROR: Must specify -i or -d.\n";
         std::exit(1);
     }
     if(opts.inputPath.empty()){
         std::cerr<<"ERROR: missing input path.\n";
         std::exit(1);
     }
     if(opts.outputPath.empty()){
         std::cerr<<"ERROR: missing output path.\n";
         std::exit(1);
     }
 
     return opts;
 }
 
