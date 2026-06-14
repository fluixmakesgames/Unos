void Ed(int pid) {
  Serial.print("File?");
  while(!Serial.available()) { } // Wait..
  String fname = Serial.readString();
  if(!FileExists(fname.c_str())) CreateFile(fname.c_str());
  Serial.print("$ ");
  while(!Serial.available()) { } // Wait again..
  String content = Serial.readString();
  WriteFile(fname.c_str(), content.c_str(), content.length());
  KillTask(pid);
}
