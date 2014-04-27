
Option Explicit On

Imports System
Imports System.Net
Imports System.Net.Sockets


Public Class clsPing

    Public Structure stcError
        Dim Number As Integer
        Dim Description As String
    End Structure

    Private Const PING_ERROR_BASE As Long = &H8000

    Public Const PING_SUCCESS As Long = 0
    Public Const PING_ERROR As Long = (-1)
    Public Const PING_ERROR_HOST_NOT_FOUND As Long = PING_ERROR_BASE + 1
    Public Const PING_ERROR_SOCKET_DIDNT_SEND As Long = PING_ERROR_BASE + 2
    Public Const PING_ERROR_HOST_NOT_RESPONDING As Long = PING_ERROR_BASE + 3
    Public Const PING_ERROR_TIME_OUT As Long = PING_ERROR_BASE + 4

    Private Const ICMP_ECHO As Integer = 8
    Private Const SOCKET_ERROR As Integer = -1

    Private udtError As stcError

    Private Const intPortICMP As Integer = 7
    Private Const intBufferHeaderSize As Integer = 8
    Private Const intPackageHeaderSize As Integer = 28

    Private byteDataSize As Byte
    Private lngTimeOut As Long
    Private ipheLocalHost As System.Net.IPHostEntry
    Private ipheHost As System.Net.IPHostEntry

    Public Property TimeOut() As Long
        Get
            Return lngTimeOut
        End Get
        Set(ByVal Value As Long)
            lngTimeOut = Value
        End Set
    End Property

    Public Property DataSize() As Byte
        Get
            Return byteDataSize
        End Get
        Set(ByVal Value As Byte)
            byteDataSize = Value
        End Set
    End Property

    Public ReadOnly Property Identifier() As Byte
        Get
            Return 0
        End Get
    End Property

    Public ReadOnly Property Sequence() As Byte
        Get
            Return 0
        End Get
    End Property

    Public ReadOnly Property LocalHost() As System.Net.IPHostEntry
        Get
            Return ipheLocalHost
        End Get
    End Property

    Public Property Host() As Object
        Get
            Return ipheHost
        End Get
        Set(ByVal Value As Object)
            If (Value.GetType.ToString.ToLower = "system.string") Then
                '*
                '* Find the Host's IP address
                '*
                Try
                    ipheHost = System.Net.Dns.GetHostByName(Value)
                Catch
                    ipheHost = Nothing
                    udtError.Number = PING_ERROR_HOST_NOT_FOUND
                    udtError.Description = "Host " & ipheHost.HostName & " not found."
                End Try
            ElseIf (Value.GetType.ToString.ToLower = "system.net.iphostentry") Then
                ipheHost = (Value)
            Else
                ipheHost = Nothing
            End If
        End Set
    End Property

    '*
    '* Class Constructor
    '*
    Public Sub New()
        '*
        '* Initializes the parameters
        '*
        byteDataSize = 32
        lngTimeOut = 500
        udtError = New stcError()

        '*
        '* Get local IP and transform in EndPoint
        '*
        ipheLocalHost = System.Net.Dns.GetHostByName(System.Net.Dns.GetHostName())

        '*
        '* Defines Host
        '*
        ipheHost = Nothing

    End Sub


    Public Function Ping() As Long

        '' Dim intCount As Integer
        Dim aReplyBuffer(255) As Byte

        Dim intNBytes As Integer = 0

        Dim intEnd As Integer
        Dim intStart As Integer

        Dim epFrom As System.Net.EndPoint
        Dim epServer As System.Net.EndPoint
        Dim ipepServer As System.Net.IPEndPoint

        '*
        '* Transforms the IP address in EndPoint
        '*
        ipepServer = New System.Net.IPEndPoint(ipheHost.AddressList(0), 0)
        epServer = CType(ipepServer, System.Net.EndPoint)

        epFrom = New System.Net.IPEndPoint(ipheLocalHost.AddressList(0), 0)

        '*
        '* Builds the packet to send
        '*
        DataSize = Convert.ToByte(DataSize + intBufferHeaderSize)

        '*
        '* The packet must by an even number, so if the DataSize is and odd number adds one 
        '* 
        If (DataSize Mod 2 = 1) Then
            DataSize += Convert.ToByte(1)
        End If
        Dim aRequestBuffer(DataSize - 1) As Byte

        '*
        '* --- ICMP Echo Header Format ---
        '* (first 8 bytes of the data buffer)
        '*
        '* Buffer (0) ICMP Type Field
        '* Buffer (1) ICMP Code Field
        '*     (must be 0 for Echo and Echo Reply)
        '* Buffer (2) checksum hi
        '*     (must be 0 before checksum calc)
        '* Buffer (3) checksum lo
        '*     (must be 0 before checksum calc)
        '* Buffer (4) ID hi
        '* Buffer (5) ID lo
        '* Buffer (6) sequence hi
        '* Buffer (7) sequence lo
        '* Buffer (8)..(n)  Ping Data
        '*

        '*
        '* Set Type Field
        '*
        aRequestBuffer(0) = Convert.ToByte(8) ' ECHO Request

        '*
        '* Set ID field
        '*
        BitConverter.GetBytes(Identifier).CopyTo(aRequestBuffer, 4)

        '*
        '* Set Sequence
        '*
        BitConverter.GetBytes(Sequence).CopyTo(aRequestBuffer, 6)

        '*
        '* Load some data into buffer
        '*
        Dim i As Integer
        For i = 8 To DataSize - 1
            aRequestBuffer(i) = Convert.ToByte(i Mod 8)
        Next i

        '*
        '* Calculate Checksum
        '*
        Call CreateChecksum(aRequestBuffer, DataSize, aRequestBuffer(2), aRequestBuffer(3))


        '*
        '* Try send the packet
        '*
        Try
            '*
            '* Create the socket
            '*
            Dim sckSocket As New System.Net.Sockets.Socket( _
                                            Net.Sockets.AddressFamily.InterNetwork, _
                                            Net.Sockets.SocketType.Raw, _
                                            Net.Sockets.ProtocolType.Icmp)
            sckSocket.Blocking = False

            '*
            '* Sends Package
            '*
            sckSocket.SendTo(aRequestBuffer, 0, DataSize, SocketFlags.None, ipepServer)

            '*
            '* Records the Start Time, after sending the Echo Request
            '*
            intStart = System.Environment.TickCount

            '*
            '* Waits for response
            '*
            Do
                Application.DoEvents()
                Try
                    intNBytes = sckSocket.ReceiveFrom(aReplyBuffer, SocketFlags.None, epServer)
                Catch objErr As Exception
                End Try
            Loop Until (intNBytes > 0) Or ((System.Environment.TickCount - intStart) > TimeOut)

            '*
            '* Check to see if the TimeOut was hit
            '*
            If ((System.Environment.TickCount - intStart) > TimeOut) Then
                udtError.Number = PING_ERROR_TIME_OUT
                udtError.Description = "Time Out"
                Return (PING_ERROR)
            End If

            '*
            '* Records End Time
            '*
            intEnd = System.Environment.TickCount

            If (intNBytes > 0) Then
                '*
                '* Informs on GetLastError the state of the server
                '*
                udtError.Number = (aReplyBuffer(19) * &H100) + aReplyBuffer(20)
                Select Case aReplyBuffer(20)
                    Case 0 : udtError.Description = "Success"
                    Case 1 : udtError.Description = "Buffer too Small"
                    Case 2 : udtError.Description = "Destination Unreahable"
                    Case 3 : udtError.Description = "Dest Host Not Reachable"
                    Case 4 : udtError.Description = "Dest Protocol Not Reachable"
                    Case 5 : udtError.Description = "Dest Port Not Reachable"
                    Case 6 : udtError.Description = "No Resources Available"
                    Case 7 : udtError.Description = "Bad Option"
                    Case 8 : udtError.Description = "Hardware Error"
                    Case 9 : udtError.Description = "Packet too Big"
                    Case 10 : udtError.Description = "Reqested Timed Out"
                    Case 11 : udtError.Description = "Bad Request"
                    Case 12 : udtError.Description = "Bad Route"
                    Case 13 : udtError.Description = "TTL Exprd In Transit"
                    Case 14 : udtError.Description = "TTL Exprd Reassemb"
                    Case 15 : udtError.Description = "Parameter Problem"
                    Case 16 : udtError.Description = "Source Quench"
                    Case 17 : udtError.Description = "Option too Big"
                    Case 18 : udtError.Description = "Bad Destination"
                    Case 19 : udtError.Description = "Address Deleted"
                    Case 20 : udtError.Description = "Spec MTU Change"
                    Case 21 : udtError.Description = "MTU Change"
                    Case 22 : udtError.Description = "Unload"
                    Case Else : udtError.Description = "General Failure"
                End Select
            End If

            Return (intEnd - intStart)
        Catch oExcept As Exception
            '
        End Try
        Return (intEnd - intStart)
    End Function

    Public Function GetLastError() As stcError
        Return udtError
    End Function

    ' ICMP requires a checksum that is the one's
    ' complement of the one's complement sum of
    ' all the 16-bit values in the data in the
    ' buffer.
    ' Use this procedure to load the Checksum
    ' field of the buffer.
    ' The Checksum Field (hi and lo bytes) must be
    ' zero before calling this procedure.
    Private Sub CreateChecksum(ByRef data() As Byte, ByVal Size As Integer, ByRef HiByte As Byte, ByRef LoByte As Byte)
        Dim i As Integer
        Dim chk As Integer = 0

        For i = 0 To Size - 1 Step 2
            chk += Convert.ToInt32(data(i) * &H100 + data(i + 1))
        Next

        chk = Convert.ToInt32((chk And &HFFFF&) + Fix(chk / &H10000&))
        chk += Convert.ToInt32(Fix(chk / &H10000&))
        chk = Not (chk)

        HiByte = Convert.ToByte((chk And &HFF00) / &H100)
        LoByte = Convert.ToByte(chk And &HFF)
    End Sub

End Class

