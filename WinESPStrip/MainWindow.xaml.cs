using System;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;



namespace WinESPStrip
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		static int Port = 53686;
		string ESPStripHost = "ESPStrip.local";
		string EspIP;

		string buffCommand;

		UdpClient Client = new UdpClient(Port);

        int Command;
		int r;
		int g;
		int b;
		int s;
		int e;
		int brg;
		int a;
		int arg;


		private void onLoad(object sender, RoutedEventArgs e)
		{
			ConnectToESP();
		}

		private void SendUDPToLED(byte[] command)
		{
			if (EspIP != null)
			{
				Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

				IPAddress serverAddr = IPAddress.Parse(EspIP);

				IPEndPoint endPoint = new IPEndPoint(serverAddr, Port);
				byte[] send_buffer = command;
				sock.SendTo(send_buffer, endPoint);
			}
			else
			{
				MessageBox.Show("Fail");
			}
		}

		private void ConnectToESP()
		{
			Log("Connecting to ESPStrip");
			Task.Run(() =>
			{
				try
				{
					IPAddress[] IPlist = Dns.GetHostAddresses(ESPStripHost);
					foreach (IPAddress IP in IPlist)
					{
						Console.WriteLine(IP.ToString());
						EspIP = IP.ToString();
						Dispatcher.Invoke(() => {
							Log("Connected to ESPStrip");
							Log("IP: " + EspIP);
						});
						
					}
				}
				catch (SocketException ex)
				{
					Console.WriteLine(ex.ToString());					
					Dispatcher.Invoke(() => {
						Log("Could not connect to ESPStrip");
					});
				}
			});
			
		}

		private byte[] CreateESPCommand(int r, int g, int b, int s, int e, int brg, int a, int arg)
		{
			byte[] Command = new byte[8] {
                Convert.ToByte(r), Convert.ToByte(g),
                Convert.ToByte(b), Convert.ToByte(s),
                Convert.ToByte(e), Convert.ToByte(brg),
                Convert.ToByte(a), Convert.ToByte(arg)
            };
			
			foreach (byte i in Command)
			{
				buffCommand += i.ToString();
			}

			Log(buffCommand);
			buffCommand = "";
			return Command;
		}

		private void Log(string i)
		{
			Logs.Items.Insert(0, i);
		}

		private void button_Click(object sender, RoutedEventArgs e)
		{
			SendUDPToLED(CreateESPCommand(255, 0, 0, 1, 60, 255, 1, 1));
			Log(((char)0).ToString() + "|");
		}

		

		private void button1_Click(object sender, RoutedEventArgs e)
		{
			string hexR = r.ToString("X2");
			string hexG = g.ToString("X2");
			string hexB = b.ToString("X2");
			Log(hexR + hexG + hexB);
		}

		private void ClrPcker_Background_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
		{
			r = Color.SelectedColor.Value.R;
			g = Color.SelectedColor.Value.G;
			b = Color.SelectedColor.Value.B;
			
			SendUDPToLED(CreateESPCommand(r, g, b, 1, 60, 255, 0, 1));		
		}

	}
}
