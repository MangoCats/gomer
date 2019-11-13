#include "dotei.h"

Dotei::Dotei(QObject *parent) : QObject(parent)
{ goishiChar = ".,XO3456789";
  Xlabels.append("A");  Ylabels.append("1");
  Xlabels.append("B");  Ylabels.append("2");
  Xlabels.append("C");  Ylabels.append("3");
  Xlabels.append("D");  Ylabels.append("4");
  Xlabels.append("E");  Ylabels.append("5");
  Xlabels.append("F");  Ylabels.append("6");
  Xlabels.append("G");  Ylabels.append("7");
  Xlabels.append("H");  Ylabels.append("8");
  Xlabels.append("J");  Ylabels.append("9");
  Xlabels.append("K");  Ylabels.append("10");
  Xlabels.append("L");  Ylabels.append("11");
  Xlabels.append("M");  Ylabels.append("12");
  Xlabels.append("N");  Ylabels.append("13");
  Xlabels.append("O");  Ylabels.append("14");
  Xlabels.append("P");  Ylabels.append("15");
  Xlabels.append("Q");  Ylabels.append("16");
  Xlabels.append("R");  Ylabels.append("17");
  Xlabels.append("S");  Ylabels.append("18");
  Xlabels.append("T");  Ylabels.append("19");
  Xlabels.append("U");  Ylabels.append("20");
  Xlabels.append("V");  Ylabels.append("21");
  Xlabels.append("W");  Ylabels.append("22");
  Xlabels.append("X");  Ylabels.append("23");
  Xlabels.append("Y");  Ylabels.append("24");
  Xlabels.append("Z");  Ylabels.append("25");
  Xlabels.append("AA"); Ylabels.append("26");  // Incompatible with GTP, but Gomer can handle them.
  Xlabels.append("AB"); Ylabels.append("27");
  Xlabels.append("AC"); Ylabels.append("28");
  Xlabels.append("AD"); Ylabels.append("29");
  Xlabels.append("AE"); Ylabels.append("30");
  Xlabels.append("AF"); Ylabels.append("31");
}

